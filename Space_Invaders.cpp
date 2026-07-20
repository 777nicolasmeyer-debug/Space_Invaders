#include "raylib.h"

struct Player {
	Vector2 position;
	float speed;
	float health;
	float dimension;
	bool active = true;
	Texture2D texture;
};

struct Bullet {
	Vector2 position;
	float speed;
	bool active;
	float radius;
	float damage = 25.0f;
	bool fromPlayer;
};

struct Enemy {
	Vector2 position;
	float speed;
	float health;
	bool active;
	float dimension;
	float damage = 25.0f;
	float shootTimer = (float)GetRandomValue(1, 4);
	Texture2D texture;
};

struct Explosion {
	Vector2 position;
	float timer;
	bool active;
};

int main()
{
	InitWindow(800, 800, "Space invaders");
	InitAudioDevice();

	const int maxEnemy = 18;
	Enemy enemies[maxEnemy] = { 0 };
	int enemyCount = 0;
	int waveNumber = 1;
	const int MAX_BULLETS = 20000;
	Bullet bullets[MAX_BULLETS] = { 0 };
	Explosion explosions[300] = { 0 };
	int explosionCount = 0;

	// Declare all sounds
	Sound blaster = LoadSound("assets/Sounds/Blaster.mp3");
	Sound explosion = LoadSound("assets/Sounds/EnemyDeath.mp3");
	Sound playerDeath = LoadSound("assets/Sounds/PlayerDeath.mp3");
	Music backgroundMusic = LoadMusicStream("assets/Sounds/Cockpit_Audio.mp3");
	SetSoundVolume(explosion, 3.5f);
	PlayMusicStream(backgroundMusic);

	// Load textures for player and enemies, background too
	Texture2D greenAlien = LoadTexture("assets/Sprites/SpaceInvaders/greenAlien.png");
	Texture2D playerTex = LoadTexture("assets/Sprites/SpaceInvaders/player.png");
	Texture2D orangeAlien = LoadTexture("assets/Sprites/SpaceInvaders/orangeAlien.png");
	Texture2D enemyShip = LoadTexture("assets/Sprites/SpaceInvaders/enemyShip.png");
	Texture2D squid = LoadTexture("assets/Sprites/SpaceInvaders/squid.png");
	Texture2D explosionTex = LoadTexture("assets/explosion.png");

	Image bluePreview = LoadImage("assets/blue-preview.png");
	Texture2D background = LoadTextureFromImage(bluePreview);

	Player player = { {400, 600}, 6.0f, 100.0f, 50.0f, true, playerTex };
	// For drawing enemies in a grid pattern
	const int rows = 3;
	const int cols = 6;
	int index = 0;

	int bulletCount = 0;

	for (int i = 0; i < maxEnemy; i++) {
		enemies[i].active = true;
		enemies[i].health = 25.0f;
		enemies[i].texture = greenAlien;
		enemies[i].position = { 100.0f + i * 100.0f, 100.0f };
		enemies[i].dimension = 50.0f;
		enemies[i].speed = 3.5f;
		enemies[i].damage = 25.0f;
		enemyCount++;
	}

	// Grid setup
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			if (index < maxEnemy) {
				enemies[index].position = { 100.0f + col * 100.0f, 100.0f + row * 100.0f };
				index++;
			}
		}
	}

	SetTargetFPS(60);

	while (!WindowShouldClose())
	{

		if (IsKeyDown(KEY_D) && player.position.x < 800 - player.dimension) {
			player.position.x += player.speed;
		}
		if (IsKeyDown(KEY_A) && player.position.x > 0) {
			player.position.x -= player.speed;
		}
		if (IsKeyPressed(KEY_SPACE)) {
			bullets[bulletCount].position.x = player.position.x + player.dimension / 2;
			bullets[bulletCount].position.y = player.position.y;
			bullets[bulletCount].speed = 10.0f;
			bullets[bulletCount].active = true;
			bullets[bulletCount].radius = 5.0f;
			bullets[bulletCount].fromPlayer = true;
			bullets[bulletCount].damage = 25.0f;
			bulletCount++;
			PlaySound(blaster);
			
		}

		// Update bullets
		for (int i = 0; i < bulletCount; i++) {
			if (bullets[i].active) {
				bullets[i].position.y -= bullets[i].speed;
				if (bullets[i].position.y < 0) {
					bullets[i].active = false;
					bulletCount--;
				}
			}
		}

		//Decide when enemy should shoot
		for (int i = 0; i < maxEnemy; i++) {
			if (enemies[i].active) {
				enemies[i].shootTimer -= GetFrameTime();
				if (enemies[i].shootTimer <= 0) {
						bullets[bulletCount].position.x = enemies[i].position.x + enemies[i].dimension / 2;
						bullets[bulletCount].position.y = enemies[i].position.y + enemies[i].dimension;
						bullets[bulletCount].speed = -10.0f; // Negative speed for downward movement
						bullets[bulletCount].active = true;
						bullets[bulletCount].radius = 5.0f;
						bullets[bulletCount].fromPlayer = false;
						bullets[bulletCount].damage = enemies[i].damage;
						bulletCount++;
						PlaySound(blaster); // Play shooting sound
						enemies[i].shootTimer = 5.0f; // Reset shoot timer
				}
			}
		}

		// Delete enemy if hit by Bullet and Health = 0
		for (int b = 0; b < bulletCount; b++) {
			for (int e = 0; e < maxEnemy; e++) {
				if (bullets[b].active && enemies[e].active && bullets[b].fromPlayer) {
					if (CheckCollisionCircleRec(bullets[b].position, bullets[b].radius, { enemies[e].position.x, enemies[e].position.y, enemies[e].dimension, enemies[e].dimension })) {
						bullets[b].active = false;
						enemies[e].health -= bullets[b].damage;
						if (enemies[e].health <= 0) {
							enemies[e].active = false;
							enemyCount--;
							player.health += 10.0f; // Increase player health by 10 points
							// Animated Explosion
							explosions[explosionCount].position = { enemies[e].position.x + enemies[e].dimension / 2,
							enemies[e].position.y + enemies[e].dimension / 2 };
							explosions[explosionCount].timer = 0.3f; // lasts 0.3 seconds
							explosions[explosionCount].active = true;
							explosionCount++;
							PlaySound(explosion);
						}
					}
								}
							}
						}

						// Handle wave progression
						if (enemyCount <= 0 && waveNumber < 4) {
							waveNumber++;
							for (int i = 0; i < maxEnemy; i++) {
								enemies[i].active = true;
								enemies[i].position = { 100.0f + (i % cols) * 100.0f, 100.0f + (i / cols) * 100.0f };
								enemies[i].speed = 3.5f + (waveNumber - 1) * 0.5f; // Increase speed with waves
								enemies[i].shootTimer = (float)GetRandomValue(1, 4);

								// Update health and texture based on wave
								if (waveNumber == 2) {
									enemies[i].health = 50.0f;
									enemies[i].texture = orangeAlien;
									enemies[i].damage = 50.0f; // Increase damage for wave 2
								}
								else if (waveNumber == 3) {
									enemies[i].health = 75.0f;
									enemies[i].texture = enemyShip;
									enemies[i].damage = 75.0f; // Increase damage for wave 3
								}
								else if (waveNumber == 4) {
									enemies[i].health = 100.0f;
									enemies[i].texture = squid;
									enemies[i].damage = 100.0f; // Increase damage for wave 4
								}
							}
							enemyCount = maxEnemy;
						}

			for (int i = 0; i < bulletCount; i++) {
				if (bullets[i].active && !bullets[i].fromPlayer) {
					if (CheckCollisionCircleRec(bullets[i].position, bullets[i].radius, { player.position.x, player.position.y, player.dimension, player.dimension })) {
						bullets[i].active = false;
						player.health -= bullets[i].damage;
						if (player.health <= 0) {
							player.active = false;
							PlaySound(playerDeath);
							for (int j = 0; j < maxEnemy; j++) {
								enemies[j].active = false;
							}
					}
				}
			}
		}
		
		// Make enemies move
		for (int i = 0; i < maxEnemy; i++) {
			if (enemies[i].active) {
				enemies[i].position.x += enemies[i].speed;
				if (enemies[i].position.x <= 0 || enemies[i].position.x + enemies[i].dimension >= 800) {
					enemies[i].speed *= -1; // Change direction
					enemies[i].position.y += 10; // Move down when changing direction, to create urgency
				}
			}
		}

		BeginDrawing();
		ClearBackground(BLACK);

		DrawTextureEx(background,  { 0, 0 }, 0.0f,2.0, WHITE);
		UpdateMusicStream(backgroundMusic);
		DrawText(TextFormat("Player Health: %.0f", player.health), 10, 10, 20, GREEN);
		if (player.active) {
			DrawTextureEx(player.texture, player.position, 0.0f, 1.5f, WHITE);

			for (int i = 0; i < maxEnemy; i++) {
				if (enemies[i].active) {
					DrawTextureEx(enemies[i].texture, enemies[i].position, 0.0f, 1.5f, WHITE);
				}
			}

			// Draw bullets
			for (int i = 0; i < bulletCount; i++) {
				if (bullets[i].active) {
					DrawCircle(bullets[i].position.x, bullets[i].position.y, bullets[i].radius, RED);
				}
			}
			for (int i = 0; i < explosionCount; i++) {
				if (explosions[i].active) {
					explosions[i].timer -= GetFrameTime();
					if (explosions[i].timer > 0) {
						DrawTextureEx(explosionTex, explosions[i].position, 0.0f, 0.1f, WHITE);
					}
					else {
						explosions[i].active = false;
					}
				}
			}
		}
		else {
			DrawText("Game Over", 350, 400, 20, RED);
		}

		EndDrawing();
	}

	// Cleanup audio
	UnloadSound(blaster);
	UnloadSound(explosion);
	UnloadSound(playerDeath);
	UnloadMusicStream(backgroundMusic);
	CloseAudioDevice();

	CloseWindow();

	return 0;
}