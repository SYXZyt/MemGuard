#include <MemGuard/MemGuard.h>
#include <stdbool.h>

typedef struct Vec2
{
	float x;
	float y;
} Vec2;

int main(int argc, char** argv)
{
	memguard_Init(true);

	Vec2* vec = mgMalloc(sizeof(Vec2));
	Vec2* vec2 = mgMalloc(sizeof(Vec2));

	int* array = mgCalloc(10, sizeof(int));

	mgFree(vec);
	mgFree(array);

	//Reports one leak from vec2
	memguard_Shutdown();

	return 0;
}