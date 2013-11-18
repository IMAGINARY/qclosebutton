#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include "qclosebutton.h"

QApplication *app;
 
static pid_t child;
void sig_handler(int sig)
{
        signal(sig, SIG_IGN);
        exit(kill(-child, sig));
}

TranslucentRoundWindow::TranslucentRoundWindow( const char* png_filename, QWidget *parent ) : QWidget(parent, Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_TranslucentBackground, true );
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint );
	
    img.load( png_filename );
    double sx = QApplication::desktop()->availableGeometry(this).width() / 1920.0;
    double sy = QApplication::desktop()->availableGeometry(this).height() / 1080.0;
    double s = sx < sy ? sx : sy;
    img = img.scaledToWidth( (int) ( s * img.width() ), Qt::SmoothTransformation ); 	
	
    resize( img.size() );
    ensurePolished();
    updatePosition();
	
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(testForAliveChild()));
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePosition()));
    connect(timer, SIGNAL(timeout()), this, SLOT(raise()));
    timer->start(200);
}

QSize TranslucentRoundWindow::sizeHint() const
{
    return img.size();
}

void TranslucentRoundWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
//    painter.setBrush(QColor(255, 0, 0, 255)); painter.drawRect(0, 0, width(), height());	 // for debugging window size and position
    painter.drawImage(0,0,img);
}

void TranslucentRoundWindow::testForAliveChild()
{
	if( waitpid(child, 0, WNOHANG) ) app->quit();
}

void TranslucentRoundWindow::updatePosition()
{
	QPoint p = QApplication::desktop()->availableGeometry(this).topRight();
	this->move(p-QPoint(this->width()-1,0));
}

void TranslucentRoundWindow::mouseReleaseEvent ( QMouseEvent * event )
{
	if( QColor::fromRgba( img.pixel( event->x(), event->y() ) ).alpha() != 0 )
	{
		kill(-child, SIGTERM);
		app->quit();
	}
}

int main( int argc, char* argv[])
{
        app = new QApplication (argc, argv);
	
        if (argc < 3) {
                fprintf(stderr, "Usage: %s PNG COMMAND [ARG]...\n", argv[0]);
                fprintf(stderr, "Start COMMAND and kill it when PNG image is pressed.\n");
                return 1;
        }

        pid_t pid = fork();

        if (pid < 0)
                return 1;

        if (0 == pid) {
                argv += 2;
                setpgid(0, 0);
                execvp(argv[0], argv);
                perror(argv[0]);
                return 1;
        }
	else
	{
		child = pid;
		signal(SIGINT, sig_handler);
		signal(SIGHUP, sig_handler);
		signal(SIGTERM, sig_handler);
		
		TranslucentRoundWindow w( argv[ 1 ] );
		w.show();
		
		return app->exec();
	}
}

