#include "CoverPage.h"

void CoverPage::renderBitmapString(float x, float y, void* font, const char* string)
{
    const char* c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}

void CoverPage::renderCenteredString(float x, float y, void* font, const char* string)
{
    renderBitmapString(x, y, font, string);
}

void CoverPage::horizontal_Line(float x1, float y1, float x2, float y2)
{
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void CoverPage::verticle_Line(float x1, float y1, float x2, float y2)
{
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

///====================Cover Page========================================
void CoverPage::display()
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0f, 0.0f, 0.0f);

    renderCenteredString(330, 680, GLUT_BITMAP_TIMES_ROMAN_24,"American International University-Bangladesh");
    renderCenteredString(425, 620, GLUT_BITMAP_HELVETICA_18,"Faculty of Science & Technology");
    renderCenteredString(424, 590, GLUT_BITMAP_HELVETICA_18,"Department of Computer Science");
    renderCenteredString(425, 560, GLUT_BITMAP_HELVETICA_18,"CSE - 4118: Computer Graphics");

    glColor3f(0.0f, 0.0f, 1.0f);
    renderCenteredString(500, 530, GLUT_BITMAP_HELVETICA_18, "Fall 2025-26");
    renderCenteredString(440, 500, GLUT_BITMAP_HELVETICA_18,"Section: M, Group Number: 01");

    glColor3f(0.0f, 0.0f, 0.0f);
    renderCenteredString(515, 460, GLUT_BITMAP_HELVETICA_18, "Submitted to");
    renderCenteredString(480, 430, GLUT_BITMAP_HELVETICA_18, "ZISHAN AHMED ONIK");
    renderCenteredString(445, 390, GLUT_BITMAP_HELVETICA_18,"Submission Date: 08 / 12 / 2025");
    renderCenteredString(200, 350, GLUT_BITMAP_HELVETICA_18, "Submitted by:");

    // Box
    horizontal_Line(200, 340, 950, 340);
    horizontal_Line(200, 310, 950, 310);
    horizontal_Line(200, 280, 950, 280);
    horizontal_Line(200, 250, 950, 250);
    horizontal_Line(200, 220, 950, 220);
    horizontal_Line(200, 190, 950, 190);
    horizontal_Line(200, 160, 950, 160);
    horizontal_Line(200, 130, 950, 130);

    verticle_Line(200, 340, 200, 130);
    verticle_Line(300, 340, 300, 130);
    verticle_Line(650, 340, 650, 130);
    verticle_Line(830, 340, 830, 130);
    verticle_Line(950, 340, 950, 130);

    // Table headers
    renderCenteredString(225, 315, GLUT_BITMAP_HELVETICA_18, "Serial");
    renderCenteredString(450, 315, GLUT_BITMAP_HELVETICA_18, "Name");
    renderCenteredString(730, 315, GLUT_BITMAP_HELVETICA_18, "ID");
    renderCenteredString(840, 315, GLUT_BITMAP_HELVETICA_18, "Contribution");

    // Members
    renderCenteredString(235, 290, GLUT_BITMAP_HELVETICA_18, "01");
    renderCenteredString(370, 290, GLUT_BITMAP_HELVETICA_18, "ABTAHI ISLAM");
    renderCenteredString(685, 290, GLUT_BITMAP_HELVETICA_18, "23-51361-1");
    renderCenteredString(870, 290, GLUT_BITMAP_HELVETICA_18, "20%");

    renderCenteredString(235, 260, GLUT_BITMAP_HELVETICA_18, "02");
    renderCenteredString(370, 260, GLUT_BITMAP_HELVETICA_18,"---");
    renderCenteredString(685, 260, GLUT_BITMAP_HELVETICA_18, "---");
    renderCenteredString(870, 260, GLUT_BITMAP_HELVETICA_18, "20%");

    renderCenteredString(235, 230, GLUT_BITMAP_HELVETICA_18, "03");
    renderCenteredString(370, 230, GLUT_BITMAP_HELVETICA_18,"---");
    renderCenteredString(685, 230, GLUT_BITMAP_HELVETICA_18, "---");
    renderCenteredString(870, 230, GLUT_BITMAP_HELVETICA_18, "20%");


    renderCenteredString(235, 200, GLUT_BITMAP_HELVETICA_18, "04");
    renderCenteredString(370, 200, GLUT_BITMAP_HELVETICA_18,"---");
    renderCenteredString(685, 200, GLUT_BITMAP_HELVETICA_18, "---");
    renderCenteredString(870, 200, GLUT_BITMAP_HELVETICA_18, "20%");

    renderCenteredString(235, 170, GLUT_BITMAP_HELVETICA_18, "05");
    renderCenteredString(370, 170, GLUT_BITMAP_HELVETICA_18,"---");
    renderCenteredString(685, 170, GLUT_BITMAP_HELVETICA_18, "---");
    renderCenteredString(870, 170, GLUT_BITMAP_HELVETICA_18, "20%");

    renderCenteredString(235, 140, GLUT_BITMAP_HELVETICA_18, "06");
    renderCenteredString(370, 140, GLUT_BITMAP_HELVETICA_18,"---");
    renderCenteredString(685, 140, GLUT_BITMAP_HELVETICA_18, "---");
    renderCenteredString(870, 140, GLUT_BITMAP_HELVETICA_18, "20%");

    renderCenteredString(970, 50, GLUT_BITMAP_HELVETICA_18,"For N, Next Page");

    glFlush();
}
