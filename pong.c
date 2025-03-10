#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

// ball
float ball_x = WINDOW_WIDTH / 2;
float ball_y = WINDOW_HEIGHT / 2;
float ball_radius = 8.0f;

// players
float player1_y = WINDOW_HEIGHT / 2;
float player2_y = WINDOW_HEIGHT / 2;

void initialize(void);
void draw_player(float pos_x, float pos_y);
void draw_ball(float pos_x, float pos_y, float radius, int segments);
void display();
void reshape(int w, int h);

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Pong");

	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}

void initialize(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);        // don't interpolate colors (efficiency)
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	draw_ball(ball_x, ball_y, ball_radius, 100);
	draw_player(20, player1_y);
	draw_player(WINDOW_WIDTH - 30, player2_y); // account for player width
	glFlush();
}

void draw_ball(float pos_x, float pos_y, float radius, int segments)
{
	// we move the coordinate system so that (0, 0) is the ball's center
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0.0f);

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);
	float angle;
	for (int i = 0; i <= segments; ++i) {
		angle = i * 2.0f * M_PI / segments; // M_PI from math.h
		glVertex2f(radius * cos(angle), radius * sin(angle));
	}
	glEnd();
	glPopMatrix();		// clear matrix transforms
}

void draw_player(float pos_x, float pos_y)
{
	const float player_width = 10.0f;
	const float player_height = 80.0f;
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(pos_x, pos_y + player_height / 2);
	glVertex2f(pos_x + player_width, pos_y + player_height / 2);
	glVertex2f(pos_x + player_width, pos_y - player_height / 2);
	glVertex2f(pos_x, pos_y - player_height / 2);
	glEnd();
}

void reshape(int w, int h)
{
	float aspect = (float)w / (float)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// we keep the 4:3 aspect ratio by adding (pillar|letter)boxes
	if (aspect >= ASPECT_RATIO) {
		float adjusted_width = h * ASPECT_RATIO;
		float margin = (w - adjusted_width) / 2;
		glViewport(margin, 0, adjusted_width, h);
	} else {
		float adjusted_height = w / ASPECT_RATIO;
		float margin = (h - adjusted_height) / 2;
		glViewport(0, margin, w, adjusted_height);
	}
	gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
