#include <MemGuard/MemGuard.h>
#include <stdbool.h>

typedef struct Vec2
{
	float x;
	float y;
} Vec2;

int main(int argc, char** argv)
{
	memguard_Init(MG_NONE);

	Vec2* vec = memguard_Malloc(sizeof(Vec2));
	Vec2* vec2 = memguard_Malloc(sizeof(Vec2));

	int* array = memguard_Calloc(10, sizeof(int));

	memguard_Free(vec);
	memguard_Free(array);

	//Reports one leak from vec2
	memguard_Report();

	return 0;
}