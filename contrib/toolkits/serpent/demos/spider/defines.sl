/*  misc.  */

#define NUMBER_OF_SENSORS               (12)

/* internal codes */

#define LEFT                            (0)
#define CENTER                          (1)
#define RIGHT                           (2)

#define GREEN_STATUS                    (0)
#define YELLOW_STATUS                   (1)
#define RED_STATUS                      (2)

/*  verticals */

#define SENSOR_TO_TOP                   (10)
#define SENSOR_TO_SENSOR                (15)
#define SENSOR_HEIGHT                   (30)
#define CC_HEIGHT                       (40)
#define SENSOR_TO_BOTTOM                (10)

#define YMAX                            (SENSOR_TO_TOP + \
                                        (NUMBER_OF_SENSORS * SENSOR_HEIGHT) + \
                                        ((NUMBER_OF_SENSORS - 1) * \
                                        SENSOR_TO_SENSOR) + SENSOR_TO_BOTTOM)

#define TOP_TO_MID_CC                   (YMAX / 2)
#define CC_YLOC                         (TOP_TO_MID_CC - (CC_HEIGHT / 2))

/*  horizontals  */

#define SENSOR_TO_SIDE                  (60)
#define SENSOR_WIDTH                    (50)
#define CC_TO_SENSOR                    (200)
#define CC_WIDTH                        (60)
#define CC_TO_CC                        (100)
#define CC_LINE_OFFSET                  (CC_TO_CC / 2)
#define SENSOR_LINE_OFFSET              (CC_TO_CC / 2 + CC_WIDTH)

#define XMAX                            (2 * \
                                        (SENSOR_TO_SIDE + SENSOR_WIDTH + \
                                        CC_TO_SENSOR + CC_WIDTH) + CC_TO_CC)

#define SIDE_TO_MIDDLE                  (XMAX / 2)
#define CC_FROM_X                       (SIDE_TO_MIDDLE - CC_LINE_OFFSET)
#define CC_TO_X                         (SIDE_TO_MIDDLE + CC_LINE_OFFSET)

#define LEFT_SENSOR_HOOK                (SENSOR_TO_SIDE + SENSOR_WIDTH)
#define RIGHT_SENSOR_HOOK               (XMAX - LEFT_SENSOR_HOOK)
#define LEFT_CC_SENSOR_HOOK             (LEFT_SENSOR_HOOK + CC_TO_SENSOR)
#define RIGHT_CC_SENSOR_HOOK            (XMAX - LEFT_CC_SENSOR_HOOK)

#define LEFT_SENSOR_LINE_MIDPOINT       (LEFT_SENSOR_HOOK + CC_TO_SENSOR / 2)
#define RIGHT_SENSOR_LINE_MIDPOINT      (XMAX - LEFT_SENSOR_LINE_MIDPOINT)

#define CC_LINE_STUB                    (10)
