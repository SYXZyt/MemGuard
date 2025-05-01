#include <MemGuard/MemGuard.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct Vec2
{
	float x;
	float y;
} Vec2;

int main(int argc, char** argv)
{
	memguard_Init(MG_NONE);

	memguard_BeginFrame();

	Vec2* vec = memguard_Malloc(sizeof(Vec2));
	Vec2* vec2 = memguard_Malloc(sizeof(Vec2));

	int* array = memguard_Calloc(10, sizeof(int));

	memguard_Free(vec);
	memguard_Free(array);

	MemGuardFrame* frame = memguard_EndFrame();
	char* str = memguard_FrameToString(frame);

	printf("%s", str);

	memguard_FrameDestroyAll(frame);
	memguard_Free(str);

	//Reports one leak from vec2
	memguard_Report();

	return 0;
}