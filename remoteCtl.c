
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include "servo.h"

#define XBOX_TYPE_BUTTON 0x01
#define XBOX_TYPE_AXIS 0x02

#define XBOX_BUTTON_A 0x00
#define XBOX_BUTTON_B 0x01
#define XBOX_BUTTON_X 0x02
#define XBOX_BUTTON_Y 0x03
#define XBOX_BUTTON_LB 0x04
#define XBOX_BUTTON_RB 0x05
#define XBOX_BUTTON_START 0x06
#define XBOX_BUTTON_BACK 0x07
#define XBOX_BUTTON_HOME 0x08
#define XBOX_BUTTON_LO 0x09 /* 左摇杆按键 */
#define XBOX_BUTTON_RO 0x0a /* 右摇杆按键 */

#define XBOX_BUTTON_ON 0x01
#define XBOX_BUTTON_OFF 0x00

#define XBOX_AXIS_LX 0x00 /* 左摇杆X轴 */
#define XBOX_AXIS_LY 0x01 /* 左摇杆Y轴 */
#define XBOX_AXIS_RX 0x03 /* 右摇杆X轴 */
#define XBOX_AXIS_RY 0x04 /* 右摇杆Y轴 */
#define XBOX_AXIS_LT 0x02
#define XBOX_AXIS_RT 0x05
#define XBOX_AXIS_XX 0x06 /* 方向键X轴 */
#define XBOX_AXIS_YY 0x07 /* 方向键Y轴 */

#define XBOX_AXIS_VAL_UP -32767
#define XBOX_AXIS_VAL_DOWN 32767
#define XBOX_AXIS_VAL_LEFT -32767
#define XBOX_AXIS_VAL_RIGHT 32767

#define XBOX_AXIS_VAL_MIN -32767
#define XBOX_AXIS_VAL_MAX 32767
#define XBOX_AXIS_VAL_MID 0x00

#define FAULT_BIT 10000 //摇杆误差、质量太差不能正确完全归位

typedef struct xbox_map
{
    int time;
    int a;
    int b;
    int x;
    int y;
    int lb;
    int rb;
    int start;
    int back;
    int home;
    int lo;
    int ro;

    int lx;
    int ly;
    int rx;
    int ry;
    int lt;
    int rt;
    int xx;
    int yy;

} xbox_map_t;

int xbox_open(char *file_name)
{
    int xbox_fd;

    xbox_fd = open(file_name, O_RDONLY);
    if (xbox_fd < 0)
    {
        perror("open");
        return -1;
    }

    return xbox_fd;
}

int xbox_map_read(int xbox_fd, xbox_map_t *map)
{
    int len, type, number, value;
    struct js_event js;

    len = read(xbox_fd, &js, sizeof(struct js_event));
    if (len < 0)
    {
        perror("read");
        return -1;
    }

    type = js.type;
    number = js.number;
    value = js.value;

    map->time = js.time;

    if (type == JS_EVENT_BUTTON)
    {
        switch (number)
        {
        case XBOX_BUTTON_A:
            map->a = value;
            break;

        case XBOX_BUTTON_B:
            map->b = value;
            break;

        case XBOX_BUTTON_X:
            map->x = value;
            break;

        case XBOX_BUTTON_Y:
            map->y = value;
            break;

        case XBOX_BUTTON_LB:
            map->lb = value;
            break;

        case XBOX_BUTTON_RB:
            map->rb = value;
            break;

        case XBOX_BUTTON_START:
            map->start = value;
            break;

        case XBOX_BUTTON_BACK:
            map->back = value;
            break;

        case XBOX_BUTTON_HOME:
            map->home = value;
            break;

        case XBOX_BUTTON_LO:
            map->lo = value;
            break;

        case XBOX_BUTTON_RO:
            map->ro = value;
            break;

        default:
            break;
        }
    }
    else if (type == JS_EVENT_AXIS)
    {
        switch (number)
        {
        case XBOX_AXIS_LX:
            map->lx = 0;
            if (fabs(value) > FAULT_BIT)
                map->lx = value;
            break;

        case XBOX_AXIS_LY:
            map->ly = 0;
            if (fabs(value) > FAULT_BIT)
                map->ly = value;
            break;

        case XBOX_AXIS_RX:
            map->rx = 0;
            if (fabs(value) > FAULT_BIT)
                map->rx = value;
            break;

        case XBOX_AXIS_RY:
            map->ry = 0;
            if (fabs(value) > FAULT_BIT)
                map->ry = value;
            break;

        case XBOX_AXIS_LT:
            map->lt = value;
            break;

        case XBOX_AXIS_RT:
            map->rt = value;
            break;

        case XBOX_AXIS_XX:
            map->xx = value;
            break;

        case XBOX_AXIS_YY:
            map->yy = value;
            break;

        default:
            break;
        }
    }
    else
    {
        /* Init do nothing */
    }

    return len;
}

void xbox_close(int xbox_fd)
{
    close(xbox_fd);
    return;
}

int main(void)
{
    int xbox_fd;
    xbox_map_t map;
    int len, type;
    int axis_value, button_value;
    int number_of_axis, number_of_buttons;

    memset(&map, 0, sizeof(xbox_map_t));

    xbox_fd = xbox_open("/dev/input/js0");
    if (xbox_fd < 0)
    {
        return -1;
    }

    servoSetup();

    int p = 0;
    int p_last = -1;
    while (1)
    {
        len = xbox_map_read(xbox_fd, &map);
        if (len < 0)
        {
            usleep(10 * 1000);
            continue;
        }

        // printf("\rTime:%8d A:%d B:%d X:%d Y:%d LB:%d RB:%d start:%d back:%d home:%d LO:%d RO:%d XX:%-6d YY:%-6d LX:%-6d LY:%-6d RX:%-6d RY:%-6d LT:%-6d RT:%-6d ",
        //         map.time, map.a, map.b, map.x, map.y, map.lb, map.rb, map.start, map.back, map.home, map.lo, map.ro,
        //         map.xx, map.yy, map.lx, map.ly, map.rx, map.ry, map.lt, map.rt);

        if (map.ry > 0)
        {
            p += 10;
        }
        else if(map.ry < 0){
            p -= 10;
        }

        if(p < 0){
            p = 0;
        }else if(p >= 180){
            p = 180;
        }
        
        
        
        if(p != p_last) {
            // servoRun(p);
            printf("map.ry:%d servo_p:%d p_last:%d\r\n", map.ry, p, p_last);
            printf("servoRun(%d)", p);

            p_last = p;
            fflush(stdout);
        }
    }

    servoShutdown();
    xbox_close(xbox_fd);
    return 0;
}
