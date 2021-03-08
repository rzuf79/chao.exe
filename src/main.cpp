#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "duktape.h"
#include "chao.h"

static duk_ret_t native_print(duk_context *ctx) {
	duk_push_string(ctx, " ");
	duk_insert(ctx, 0);
	duk_join(ctx, duk_get_top(ctx) - 1);
	printf("%s\n", duk_safe_to_string(ctx, -1));
	return 0;
}

static duk_ret_t native_adder(duk_context *ctx) {
	int i;
	int n = duk_get_top(ctx);  /* #args */
	double res = 0.0;

	for (i = 0; i < n; i++) {
		res += duk_to_number(ctx, i);
	}

	duk_push_number(ctx, res);
	return 1;  /* one return value */
}

void duk_test() {
	duk_context *ctx = duk_create_heap_default();

	// (void) argc; (void) argv;  /* suppress warning */

	duk_push_c_function(ctx, native_print, DUK_VARARGS);
	duk_put_global_string(ctx, "print");
	duk_push_c_function(ctx, native_adder, DUK_VARARGS);
	duk_put_global_string(ctx, "adder");
	duk_eval_string(ctx, "print('Hail Eris!');");
	duk_eval_string(ctx, "print('2+3=' + adder(2, 3));");
	duk_pop(ctx);  /* pop eval result */

	duk_destroy_heap(ctx);
}

int main( int argc, char* args[] )
{
	duk_test();

	Chao *chao = new Chao(640, 480, Chao::SCALING_MODE_NONE);
	chao->loadImage("sticker", "assets/sticker.png");
	chao->runGameLoop();
	
	delete(chao);


	return 0;
}