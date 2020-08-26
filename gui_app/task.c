#include "task.h"
#include "lvgl.h"
#include "beep.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "delay.h"

typedef struct
{
    char name[20];
    u8 age;
} USER_DATA;

lv_task_t *task1 = NULL; //任务句柄指针
USER_DATA user_data = {
    .name = {"Zongyu Guo"},
    .age = 22}; //user_data为用户自定义参数,数据结构一般为结构体

void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (obj)
    {
        if (event == LV_EVENT_RELEASED) //触摸释放事件
        {
            DS0 = !DS0;
            printf("button click.\r\n");
        }
    }
}

//任务回调函数
void task1_cb(lv_task_t *task)
{
    USER_DATA *dat = (USER_DATA *)(task->user_data); //获取用户的自定义参数

    //打印tick时间(一个tick为1ms)和用户自定义参数
    printf("task1_cb_tick:%d,name:%s,age:%d\r\n", lv_tick_get(), dat->name, dat->age);

    //蜂鸣器鸣叫20ms进行提示
    BEEP = 1;
    DS1 = 0;
    delay_ms(200);
    BEEP = 0;
    DS1 = 1;
}

//例程入口函数
void task_test_start()
{
    //创建task1任务
    task1 = lv_task_create(task1_cb, 5000, LV_TASK_PRIO_MID, &user_data);
}

void key_handler(void)
{
    if (task1 == NULL)
        return;
    if (key_check(KEY2) == KEY_DOWN)
    {
        //删除任务,当你按下KEY2键后,你会发现task1_cb回调函数将永远不会再被执行了
        lv_task_del(task1);
        task1 = NULL;
        printf("task_del_tick:%d\r\n", lv_tick_get());
    }
}
