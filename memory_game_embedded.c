#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "string.h"

#define BL_LED 12
#define RD_LED 13
#define BTN_BL 5
#define BTN_RD 6
#define GR_LED 11

void setPIN(int input_pin, int output_pin)
{
    gpio_init(output_pin);
    gpio_init(input_pin);

    gpio_put(output_pin, false);

    gpio_set_dir(output_pin, GPIO_OUT);
    gpio_set_dir(input_pin, GPIO_IN);
    gpio_pull_up(input_pin);
}

void flip_led(int led_input, bool btn_state)
{
    gpio_put(led_input, btn_state);
}

// true yes, false no
bool is_button_pressed(int button_pin)
{
    return !gpio_get(button_pin);
}

int *get_random_order(int amount)
{
    int min = 0;
    int max = 1;
    int *order = (int *)malloc(amount * sizeof(int));
    if (!order)
        return NULL;
    for (int i = 0; i < amount; i++)
    {
        order[i] = rand() % (max + 1 - min) + min;
    }
    return order;
}

int identify_pin_button_pressed(bool blue_button, bool red_button)
{
    if (blue_button)
    {
        return 12;
    }
    else if (red_button)
    {
        return 13;
    }

    return -1;
}

int main()
{

    stdio_init_all();
    setPIN(BTN_BL, BL_LED);
    setPIN(BTN_RD, RD_LED);

    gpio_init(GR_LED);
    gpio_set_dir(GR_LED, GPIO_OUT);
    gpio_put(GR_LED, false);
    int leds[] = {BL_LED, RD_LED};
    int *order = get_random_order(5);
    bool userTurn = false;

    while (true)
    {
        // button a
        bool button_blue = is_button_pressed(BTN_BL);
        // button b
        bool button_red = is_button_pressed(BTN_RD);

        int points = 0;

        if (button_blue)
        {
            for (int i = 0; i < sizeof(order); i++)
            {
                printf("LED: %i \n", leds[order[i]]);
                flip_led(leds[order[i]], 1);
                sleep_ms(600);
                flip_led(leds[order[i]], 0);
                sleep_ms(600);
            }
            userTurn = true;

            while (userTurn)
            {
                for (int i = 0; i < sizeof(order);)
                {
                    if (points >= sizeof(order))
                    {
                        break;
                    }

                    button_blue = is_button_pressed(BTN_BL);
                    button_red = is_button_pressed(BTN_RD);
                    int button_pressed = identify_pin_button_pressed(button_blue, button_red);
                    if (button_pressed > 0)
                    {
                        if (button_pressed == leds[order[i]])
                        {
                            points += 1;
                            if (points >= sizeof(order))
                            {
                                printf("VENCEU! \n");
                                userTurn = false;
                                break;
                            }
                            i++;
                            gpio_put(GR_LED, 1);
                            sleep_ms(100);
                            gpio_put(GR_LED, 0);
                            printf("ACERTOU! \n");
                            gpio_pull_down(button_pressed);
                        }
                        else
                        {
                            i++;
                            printf("PERDEU! \n");
                        }
                        printf("ROUND: %i \n", i);
                    }
                    sleep_ms(140);
                }
                userTurn = false;
                sleep_ms(200);
            }

            for (size_t i = 0; i < 5; i++)
            {
                gpio_put(GR_LED, 1);
                sleep_ms(100);
                gpio_put(GR_LED, 0);
                sleep_ms(100);
            }

            free(order);
            order = get_random_order(5);
        }
        sleep_ms(50);
    }

    free(order);

    return 0;
}
