#include <stdio.h>
#include "chao.h"
#include "resources.h"


float ludzik_angle = 0.0f;
Vector2 sticker_pos;

void init() {
	image_smoothing_enabled = false;
	clear_color = Color(0.2f, 0.2f, 0.2f, 1.0f);
	load_base64_image("crappy_knight", crappy_knight_png);
	load_base64_image("sticker", sticker_png);
	load_base64_image("zbylut", zbylut_anim_png);
}

void update() {
	ludzik_angle += get_delta_time() * 50.f;
	draw_image("crappy_knight", screen_size- 32);

	float speed = get_delta_time() * 100.0f;
	if(pressed[KEY_UP]) sticker_pos.y -= speed;
	if(pressed[KEY_DOWN]) sticker_pos.y += speed;
	if(pressed[KEY_LEFT]) sticker_pos.x -= speed;
	if(pressed[KEY_RIGHT]) sticker_pos.x += speed;

	if(just_pressed[KEY_SPACE]) printf("pressed!\n");
	if(just_pressed[KEY_A]) printf("pressed!\n");
	if(just_released[KEY_SPACE]) printf("released!\n");

	Image* sticker = get_image("sticker");
	draw_image(*sticker, sticker_pos);

	draw_image_part(*get_image("zbylut"), Vector2(300, 100), Rect(0, 0, 32, 32), Color(), Vector2(4, 4));
}

void chao_main() {
	chao_init("Rogue souls", 640, 400, SSM_EXTEND, &init, &update);

}
	