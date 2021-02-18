.OF 'X-Window日本語Version研究会'A\-%'漢字入力システム仕様(第2版)'
.EF 'X-Window日本語Version研究会'A\-%'漢字入力システム仕様(第2版)'
.nr PI 2n
.LP
.sp 3
.SH
.LG
.LG
漢字入力システム仕様・付録
.LP
.sp 2
.RS
.RS
.IP 付録１ 10
エラー番号
.RE
.RE
.OH ''''
.EH ''''
.bp
.SH
.LG
［付録１］エラー番号
.LP
XJPライブラリのエラーハンドラ(XJp_perror)で使用するエラー番号を
以下に示す。
.LP
なお、エラー番号（数値）は、本Versionに対してのみ有効です。
.SH
１．XJPフロントエンド側でのエラー
.IP ・ 2
\fBフロントエンド自身\fP
.RS
.IP "  0\ XJP_F_FRONTEND_END" 40
フロントエンドが正常終了しました
.IP "  1\ XJP_F_FRONTEND_ERR_END" 40
フロントエンドが異常終了したました
.IP "  2\ XJP_F_JSERVER_DISCONNECT" 40
かな漢字変換サーバとの接続が切れたました
.RE
.IP ・ 2
\fBXJp_open()\fP
.RS
.IP "  3\ XJP_F_OPEN_BAD_WID" 40
仮名漢字変換要求ウィンドウIDが間違っています
.IP "  4\ XJP_F_OPEN_BAD_SZ" 40
変換ウィンドウのサイズ指定が間違っています
.IP "  5\ XJP_F_OPEN_BAD_FN" 40
フォント名の指定が間違っています
.IP "  6\ XJP_F_OPEN_BAD_DM" 40
dispmode の指定が間違っています
.IP "  7\ XJP_F_OPEN_NOT_WIN" 40
変換ウィンドウが作成できませんでした
.IP "  8\ XJP_F_OPEN_NOT_SUPPORT" 40
指定されたdispmodeはサポートされていません。
.RE
.IP ・ 2
\fBXJp_close()\fP
.RS
.IP "  9\ XJP_F_CLOSE_BAD_CL" 40
そのようなクライアントは存在しません
.RE
.IP ・ 2
\fBXJp_begin()\fP
.RS
.IP " 10\ XJP_F_BEGIN_BAD_CL" 40
そのようなクライアントは存在しません
.IP " 11\ XJP_F_BEGIN_BAD_WID" 40
キー入力ウィンドウIDが間違っています
.RE
.IP ・ 2
\fBXJp_end()\fP
.RS
.IP " 12\ XJP_F_END_BAD_CL" 40
そのようなクライアントは存在しません
.IP " 13\ XJP_F_END_BAD_WID"	40
キー入力ウィンドウIDが間違っています
.RE
.IP ・ 2
\fBXJp_visible()\fP
.RS
.IP " 14\ XJP_F_VISIBLE_BAD_CL" 40
そのようなクライアントは存在しません
.RE
.IP ・ 2
\fBXJp_invisible()\fP
.RS
.IP " 15\ XJP_F_INVISIBLE_BAD_CL" 40
そのようなクライアントは存在しません
.RE
.IP ・ 2
\fBXJp_change()\fP
.RS
.IP " 16\ XJP_F_CHANGE_BAD_CL" 40
そのようなクライアントは存在しません
.IP " 17\ XJP_F_CHANGE_BAD_SZ" 40
変換ウィンドウのサイズ指定が間違っています
.IP " 18\ XJP_F_CHANGE_NOT_WIN" 40
変換ウィンドウの属性変更ができませんでした
.RE
.SH
２．ライブラリ側でのエラー
.IP ・ 2
\fB各ライブラリ共通\fP
.RS
.IP "200\ XJP_L_NO_EXIST_FE" 40
フロントエンドが存在しません
.IP "201\ XJP_L_BAD_REQUEST_ID" 40
引数request_IDが間違っています
.IP "202\ XJP_L_BAD_W_KEY" 40
引数w_keyが間違っています
.RE
.IP ・ 2
\fBXJp_open()\fP
.RS
.IP "203\ XJP_L_OPEN_BAD_DPY" 40
引数displayが間違っています
.IP "204\ XJP_L_OPEN_BAD_FN" 40
フォント名の指定が間違っています
.IP "205\ XJP_L_OPEN_BAD_DM" 40
引数dispmodeが間違っています
.RE
.IP ・ 2
\fBXJp_read()\fP
.RS
.IP "206\ XJP_L_READ_NOT_CL_EV" 40
引数xEvのイベントタイプがClientMessageではありません
.IP "207\ XJP_L_READ_NOT_FROM_FE" 40
引数xEvのイベントがフロントエンドからのものではありません
.IP "208\ XJP_L_READ_OVERFLLOW" 40
引数sizeが小さ過ぎます
.IP "209\ XJP_L_READ_BAD_DATA" 40
引数xEvのイベント内に不正なデータがあります
.RE
.IP ・ 2
\fBXJp_string()\fP
.RS
.IP "210\ XJP_L_STRING_NOT_CL_EV" 40
引数xEvのイベントタイプがClientMessageではありません
.IP "211\ XJP_L_STRING_NOT_FROM_FE" 40
引数xEvのイベントがフロントエンドからのものではありません
.IP "212\ XJP_L_STRING_OVERFLLOW" 40
引数sizeが小さ過ぎます
.IP "213\ XJP_L_STRING_BAD_CODE" 40
引数codeが間違っています
.IP "214\ XJP_L_STRING_BAD_DATA" 40
引数xEvのイベント内に不正なデータがあります
.RE
.IP ・ 2
\fBXJp_reset()\fP
.RS
.IP "215\ XJP_L_RESET_NOT_DN_EV" 40
引数eventのイベントタイプがDestroyNotifyではありません
.IP "216\ XJP_L_RESET_NOT_FROM_CW" 40
引数eventがかな漢字変換ウィンドウのものではありません
.RE
.IP ・ 2
\fBXLookupKanjiString()\fP
.RS
.IP "200\ XJP_L_NO_EXIST_FE" 40
フロントエンドが存在しません
.IP "401\ XJP_L_BAD_SOCK" 40
ソケットが作れません
.IP "402\ XJP_L_NOT_CONN" 40
コネクトできません
.IP "403\ XJP_L_ERR_WRITE" 40
writeが失敗しました
.IP "404\ XJP_L_ERR_INTR" 40
フロントエンドが返答しません
.IP "405\ XJP_L_ERR_MISC" 40
その他のエラー
