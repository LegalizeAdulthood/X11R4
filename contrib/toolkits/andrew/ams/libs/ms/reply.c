/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/reply.c,v 2.7 89/04/16 11:30:53 nsb Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/reply.c,v $
*/
#include <stdio.h>
#include <ms.h>
#include <hdrparse.h>
#include <andrewos.h> /* sys/file.h */

extern FILE *fopen();
extern char *malloc(), *RewriteSubject(), *index(), *rindex(), *StripWhiteEnds();
extern char MeInFull[];
static char *ForwardString = "---------- Forwarded message begins here ----------\n\n";

MS_NameReplyFile(DirName, id, code, FileName)
char *DirName, *id, *FileName; 
	/* The first two char * arguments are passed in, the last returned */
int code; /* passed in */
{
    struct MS_Directory *Dir;
    struct MS_Message *Msg;
    FILE *fp;
    char *Subject= NULL, RawFileName[MAXPATHLEN+1], *CC=NULL, *TmpCC, *InReplyTo = NULL, BE2Format[20], *References = NULL, *Distribution = NULL, BigBuf[5000];
    int len, skiplen, FormatAsInt, reflen, IsBE2;
    int bytesleft, bytestoread;

    debug(1, ("MS_NameReplyFile %s %s %d\n", DirName ? DirName : "", id ? id : "", code));
    BE2Format[0] = '\0';
    GenTempName(FileName);
    fp = fopen(FileName, "w");
    if (fp == NULL) {
	AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_NAMEREPLYFILE);
    }
    if ((Msg = (struct MS_Message *) malloc (sizeof (struct MS_Message))) == NULL) {
	fclose(fp);
	AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_NAMEREPLYFILE);
    }
    bzero(Msg, sizeof(struct MS_Message));
    Msg->OpenFD = -1;
    if (!id || !DirName || !*id || !*DirName) {
	code = AMS_REPLY_FRESH;
    }
    if (code == AMS_REPLY_FRESH) {
	Msg->Snapshot =  malloc(AMS_SNAPSHOTSIZE);
	if (Msg->Snapshot == NULL) {
	    fclose(fp);
	    FreeMessage(Msg, TRUE);
	    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_NAMEREPLYFILE);
	}
	InventID(Msg);
    } else {
	debug(4, ("Parsing message\n"));
	if (ReadOrFindMSDir(DirName, &Dir, MD_OK) != 0) {
	    fclose(fp);
	    FreeMessage(Msg, TRUE);
	    return(mserrcode);
	}
	GetBodyFileName(Dir->UNIXDir, id, RawFileName);
	if (ReadRawFile(RawFileName, Msg, FALSE)
	    || ParseMessageFromRawBody(Msg)
	    || InventID(Msg)
	    || BuildReplyField(Msg)
	    || BuildWideReply(Msg, code == AMS_REPLY_WIDER ? TRUE : FALSE))
	{
	    FreeMessage(Msg, TRUE);
	    fclose(fp);
	    return(mserrcode); /* already set */
	}
	if (code == AMS_REPLY_FORWARD) {
	    if (UnformatMessage(Msg)) {
		FreeMessage(Msg, TRUE);
		fclose(fp);
		return(mserrcode);
	    }
	}
	len = Msg->ParsedStuff->HeadBodyLen[HP_SUBJECT];
	Subject = malloc(10 + len);
	if (Subject == NULL) {
	    fclose(fp);
	    FreeMessage(Msg, TRUE);
	    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_NAMEREPLYFILE);
	}
	strcpy(Subject, ((code == AMS_REPLY_FORWARD) || (code == AMS_REPLY_FORWARD_FMT)) ? "Fwd: " : "Re: ");
	if (len > 0) {
	    char *NewSub;

	    strncat(Subject, Msg->ParsedStuff->HeadBody[HP_SUBJECT], len);
	    NewSub = RewriteSubject(Subject);
	    free(Subject);
	    if (!NewSub) {
		fclose(fp);
		FreeMessage(Msg, TRUE);
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_NAMEREPLYFILE);
	    }
	    Subject = NewSub;
	}
	len = Msg->ParsedStuff->HeadBodyLen[HP_CC];
	CC = malloc(5 + len);
	if (CC == NULL) {
	    fclose(fp);
	    free(Subject);
	    FreeMessage(Msg, TRUE);
	    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_NAMEREPLYFILE);
	}
	strncpy(CC, Msg->ParsedStuff->HeadBody[HP_CC], len);
	CC[len] = '\0';
	if (!StripMyselfFromAddressList(CC, &TmpCC)) {
	    free(CC);
	    CC = TmpCC;
	}
	len = Msg->ParsedStuff->HeadBodyLen[HP_DISTRIBUTION];
	if (len) {
	    Distribution = malloc(20+len);
	    if (!Distribution) {
		fclose(fp);
		free(Subject);
		free(CC);
		FreeMessage(Msg, TRUE);
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_NAMEREPLYFILE);
	    }
	    strcpy(Distribution, "\nDistribution: ");
	    reflen = strlen(Distribution);
	    strncpy(Distribution+reflen, Msg->ParsedStuff->HeadBody[HP_DISTRIBUTION], len);
	    Distribution[len+reflen] = '\0';
	}
	len = Msg->ParsedStuff->HeadBodyLen[HP_MESSAGEID];
	reflen =  Msg->ParsedStuff->HeadBodyLen[HP_REFERENCES];
	if (len + reflen > 0) {
	    References = malloc(10+len+reflen);
	    if (!References) {
		fclose(fp);
		if (Subject) free(Subject);
		if (Distribution) free(Distribution);
		free(CC);
		FreeMessage(Msg, TRUE);
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_NAMEREPLYFILE);
	    }
	    if (reflen) {
		strncpy(References, Msg->ParsedStuff->HeadBody[HP_REFERENCES], reflen);
		References[reflen] = '\0';
	    } else {
		*References = '\0';
	    }
	    if (len) {
		char *s;

		if (reflen) {
		    strcat(References, ",\n\t");
		}
		s = References + strlen(References);
		strncpy(s, Msg->ParsedStuff->HeadBody[HP_MESSAGEID], len);
		s[len] = '\0';
	    }
	}
	if (len) {
	    InReplyTo = malloc(5+len);
	    if (InReplyTo == NULL) {
		fclose(fp);
		free(Subject);
		free(CC);
		FreeMessage(Msg, TRUE);
		if (Distribution) free(Distribution);
		if (References) free (References);
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_NAMEREPLYFILE);
	    }
	    strncpy(InReplyTo, Msg->ParsedStuff->HeadBody[HP_MESSAGEID], len);
	    InReplyTo[len] = '\0';
	} else {
	    /* No message id, generate my own in-reply-to text & no "references"  */
	    char *s;

	    len = Msg->ParsedStuff->HeadBodyLen[HP_DATE];
	    if (len <= 0) len = 30;
	    len += 50 + (Msg->ReplyTo ? strlen(Msg->ReplyTo) : 0);
	    InReplyTo = malloc(len);
	    if (InReplyTo == NULL) {
		fclose(fp);
		free(Subject);
		free(CC);
		FreeMessage(Msg, TRUE);
		if (Distribution) free(Distribution);
		if (References) free (References);
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_NAMEREPLYFILE);
	    }
	    sprintf(InReplyTo, "Message from \377%s\377 dated \377", Msg->ReplyTo ? Msg->ReplyTo : "<someone>");
	    len = Msg->ParsedStuff->HeadBodyLen[HP_DATE];
	    if (len) {
		strncat(InReplyTo, Msg->ParsedStuff->HeadBody[HP_DATE], len);
		strcat(InReplyTo, "\377");
	    } else {
		strcat(InReplyTo, "<NO DATE>\377");
	    }
	    for (s = InReplyTo; *s; ++s) {
		switch (*s) {
		    case '\"':
		    case '\n':
		    case '\\':
			*s = ' ';
			break;
		    case '\377':
			*s = '\"';
			break;
		    default:
			break;
		}
	    }
	}
    }
    debug(4, ("Writing out file\n"));
    switch(code) {
	case AMS_REPLY_ACK:
	    EmitHeader(Msg, HP_ACKTO, fp, "To: ");
	    EmitHeader(Msg, HP_SUBJECT, fp, "Subject: Confirming: ");
	    fprintf(fp, "Ack-type: explicit\nAck: ");
	    if (Msg->ParsedStuff->HeadBody[HP_MESSAGEID]) {
		fwriteallchars(Msg->ParsedStuff->HeadBody[HP_MESSAGEID], Msg->ParsedStuff->HeadBodyLen[HP_MESSAGEID], fp);
	    } else {
		fprintf(fp, "<No-Message-ID-In-Ack-Request>");
	    }		
	    fputs("\n\n", fp);
	    fprintf(fp, "This message confirms the receipt by ``%s''\nof a message ", MeInFull);
	    if (Msg->ParsedStuff->HeadBody[HP_FROM]) {
		fputs("from ``", fp);
		fwriteallchars(Msg->ParsedStuff->HeadBody[HP_FROM], Msg->ParsedStuff->HeadBodyLen[HP_FROM], fp);
		fputs("''", fp);
	    } else {
		fputs("with no ``from'' header", fp);
	    }
	    if (Msg->ParsedStuff->HeadBody[HP_SUBJECT]) {
		fprintf(fp, "\non the subject ``");
		fwriteallchars(Msg->ParsedStuff->HeadBody[HP_SUBJECT], Msg->ParsedStuff->HeadBodyLen[HP_SUBJECT], fp);
		fputs("''", fp);
	    } else {
		fprintf(fp, "\nwith no ``subject'' header");
	    }
	    if (Msg->ParsedStuff->HeadBody[HP_DATE]) {
		fprintf(fp, "\ndated ``");
		fwriteallchars(Msg->ParsedStuff->HeadBody[HP_DATE], Msg->ParsedStuff->HeadBodyLen[HP_DATE], fp);
		fputs("''.", fp);
	    } else {
		fprintf(fp, "\nwith no ``date'' header.");
	    }
	    fprintf(fp, "\n\nThis is an ``explicit'' receipt, which means that the user\nsaw the message and agreed to send this confirmation.\n");
	    break;
	case AMS_REPLY_REDRAFT:
	    EmitHeader(Msg, HP_SCRIBEFORMAT, fp, "X-Andrew-ScribeFormat: ");
	    EmitHeader(Msg, HP_CONTENTTYPE, fp, "Content-Type: ");
	    EmitHeader(Msg, HP_UNSUPPORTEDTYPE, fp, "If-Type-Unsupported: ");
	    EmitHeader(Msg, HP_TO, fp, "To: ");
	    EmitHeader(Msg, HP_SUBJECT, fp, "Subject: ");
	    EmitHeader(Msg, HP_CC, fp, "CC: ");
	    EmitHeader(Msg, HP_REPLY_TO, fp, "Reply-To: ");
	    EmitHeader(Msg, HP_SENDER, fp, "Sender: ");
	    EmitHeader(Msg, HP_ENCLOSURE, fp, "Enclosure: ");
	    EmitHeader(Msg, HP_WIDEREPLY, fp, "X-Andrew-WideReply: ");
	    EmitHeader(Msg, HP_DIRECTORYCREATION, fp, "X-Andrew-DirectoryCreation: ");
	    EmitHeader(Msg, HP_VOTEREQUEST, fp, "Vote-Request: ");
	    EmitHeader(Msg, HP_VOTETO, fp, "Vote-To: ");
	    EmitHeader(Msg, HP_VOTECHOICES, fp, "Vote-Choices: ");
	    EmitHeader(Msg, HP_INREPLYTO, fp, "In-Reply-To: ");
	    EmitHeader(Msg, HP_REFERENCES, fp, "References: ");
	    EmitHeader(Msg, HP_DISTRIBUTION, fp, "Distribution: ");
	    fputc('\n', fp);
	    lseek(Msg->OpenFD, Msg->BodyOffsetInFD, L_SET);
	    bytesleft = Msg->FullSize - Msg->HeadSize;
	    while (bytesleft > 0) {
		bytestoread = (bytesleft > (sizeof(BigBuf)-1)) ? (sizeof(BigBuf) - 1) : bytesleft;
		if ((read(Msg->OpenFD, BigBuf, bytestoread) != bytestoread) || (fwriteallchars(BigBuf, bytestoread, fp) != bytestoread)) {
		    fclose(fp);
		    free(Subject);
		    free(CC);
		    FreeMessage(Msg, TRUE);
		    if (Distribution) free(Distribution);
		    if (References) free (References);
		    AMS_RETURN_ERRCODE(errno, EIN_READ, EVIA_NAMEREPLYFILE);
		}
		bytesleft -= bytestoread;
	    }
	    break;
	case AMS_REPLY_FRESH:
	    fprintf(fp, "To: \nSubject: \nCC: \n\n");
	    break;
	case AMS_REPLY_FORWARD:
	case AMS_REPLY_FORWARD_FMT:
	    GetFormatFromMessage(Msg, BE2Format, sizeof(BE2Format), &IsBE2);
	    if (BE2Format[0]) {
		fprintf(fp, "Content-Type: X-BE2; %s\n", BE2Format);
	    }
	    fprintf(fp, "To: \nSubject: %s\nCC: %s%s%s\n\n", Subject, References ? "\nReferences: " : "", References ? References : "", Distribution ? Distribution : "");
	    FormatAsInt = atoi(BE2Format);
	    if (FormatAsInt >= 10) { /* be2 format */
		EmitBE2PrefixAndLSeekPastIt(Msg->OpenFD, fp, &skiplen);
	    } else {
		skiplen = 0;
	    }
	    PrintQuotingFormatting(fp, ForwardString, BE2Format, strlen(ForwardString));
	    if (BE2Format[0]) {
		PrintSpecialStuff(fp, PR_STARTSHRINK, BE2Format);
		PrintSpecialStuff(fp, PR_STARTSHRINK, BE2Format);
	    }
	    PrintQuotingFormatting(fp, Msg->RawBits, BE2Format, Msg->HeadSize);
	    if (BE2Format[0]) {
		PrintSpecialStuff(fp, PR_ENDSHRINK, BE2Format);
		PrintSpecialStuff(fp, PR_ENDSHRINK, BE2Format);
	    }
	    bytesleft = Msg->FullSize - Msg->HeadSize - skiplen;
	    while (bytesleft > 0) {
		bytestoread = (bytesleft > (sizeof(BigBuf)-1)) ? (sizeof(BigBuf) - 1) : bytesleft;
		if (read(Msg->OpenFD, BigBuf, bytestoread) != bytestoread) {
		    if (Subject) free (Subject);
		    if (CC) free (CC);
		    if (InReplyTo) free(InReplyTo);
		    FreeMessage(Msg, TRUE);
		    fclose(fp);
		    if (Distribution) free(Distribution);
		    if (References) free (References);
		    AMS_RETURN_ERRCODE(errno, EIN_READ, EVIA_NAMEREPLYFILE);
		}
		if (BE2Format[0]) {
		    fwriteallchars(BigBuf, bytestoread, fp);
		} else {
		    PrintQuotingFormatting(fp, BigBuf, BE2Format, bytestoread);
		}
		bytesleft -= bytestoread;
	    }
	    PrintQuotingFormatting(fp, "\n", BE2Format, 1); /* for newline termination */
	    break;
	case AMS_REPLY_WIDE:
	case AMS_REPLY_WIDER:
	    fprintf(fp, "To: %s\nSubject: %s\nCC: %s\nIn-Reply-To: %s%s%s%s\n\n",
		Msg->WideReply ? Msg->WideReply : " ", Subject, CC, InReplyTo, References ? "\nReferences: " : "", References ? References : "", Distribution ? Distribution : "");
	    break;
	case AMS_REPLY_SENDER:
	    fprintf(fp, "To: %s\nSubject: %s\nCC: \nIn-Reply-To: %s%s%s%s\n\n", Msg->ReplyTo ? Msg->ReplyTo : " ", Subject, InReplyTo, References ? "\nReferences: " : "", References ? References : "", Distribution ? Distribution : "");
	    break;
	default:
	    if (Subject) free (Subject);
	    if (CC) free (CC);
	    if (InReplyTo) free(InReplyTo);
	    FreeMessage(Msg, TRUE);
	    fclose(fp);
	    if (Distribution) free(Distribution);
	    if (References) free (References);
	    AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_NAMEREPLYFILE);
    }
    if (Subject) free (Subject);
    if (CC) free (CC);
    if (InReplyTo) free(InReplyTo);
    if (Distribution) free(Distribution);
    if (References) free (References);
    FreeMessage(Msg, TRUE);
    if (ferror(fp) || feof(fp)) {
	fclose(fp);
	AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_NAMEREPLYFILE);
    }
    if (vfclose(fp) != 0) {
	AMS_RETURN_ERRCODE(errno, EIN_VFCLOSE, EVIA_NAMEREPLYFILE);
    }
    return(0);
}

EmitHeader(Msg, which, fp, head)
struct MS_Message *Msg;
int which;
FILE *fp;
char *head;
{
    int len;

    len = Msg->ParsedStuff->HeadBodyLen[which];
    if (len) {
	fputs(head, fp);
	fwriteallchars(Msg->ParsedStuff->HeadBody[which], len, fp);
	fputc('\n', fp);
    }
}
