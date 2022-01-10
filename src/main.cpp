#include <stdio.h>
#include "chao.h"
#include "resources.h"


float ludzik_angle = 0.0f;
Vector2 sticker_pos;
float bounce_timer = 0.f;

void init() {
	set_window_size(960, 600);
	image_smoothing_enabled = false;
	clear_color = Color(0.2f, 0.2f, 0.2f, 1.0f);
	load_base64_image("crappy_knight", crappy_knight_png);
	load_base64_image("sticker", sticker_png);
	load_base64_image("zbylut", zbylut_anim_png);
	// load_base64_image("font_8", font_8_png);
	load_base64_image("font_8", cp437_8x8_png);

	sticker_pos = screen_size / 2;
	// PlaySound(TEXT("amphibians.mp3"), NULL, SND_FILENAME | SND_ASYNC);
	// play_midi_file(NULL, "muzyczka.mid");
}

void update() {
	ludzik_angle += get_delta_time() * 50.f;
	// draw_image("crappy_knight", screen_size- 32);

	float speed = get_delta_time() * 100.0f;
	if(pressed[KEY_UP]) sticker_pos.y -= speed;
	if(pressed[KEY_DOWN]) sticker_pos.y += speed;
	if(pressed[KEY_LEFT]) sticker_pos.x -= speed;
	if(pressed[KEY_RIGHT]) sticker_pos.x += speed;

	if(just_pressed[KEY_SPACE]) {
		PlaySound(TEXT("kazoo.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}

	Image* sticker = get_image("sticker");
	draw_image(*sticker, sticker_pos, Color(), Vector2(1,1), ludzik_angle);

	draw_image_part(*get_image("zbylut"), Vector2(300, 100), Rect(0, 0, 32, 32), Color(), Vector2(4, 4));

	bounce_timer += get_delta_time() * 5.0f;
	draw_image("crappy_knight", Vector2(screen_size.x-16,screen_size.y), Color(), Vector2(1, 0.7f+0.3f*abs(sin(bounce_timer))), 0, Vector2(0.5f, 1.0f), true);
	draw_image("crappy_knight", Vector2(0,0), Color(), Vector2(1,1), 0, Vector2(0,0));

	draw_text("font_8", Vector2(100,100), Vector2(8,8), "IO CHAOS!\n(i maryja zawsze dziewica)");

}

void chao_main() {
	chao_init("Rogue souls", 320, 200, SSM_EXTEND, &init, &update);
}
	