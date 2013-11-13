#include <QWidget>
#include <QtGui> 
 
 class TranslucentRoundWindow : public QWidget
{
    Q_OBJECT
	
    public:
        TranslucentRoundWindow( const char* png_filename, QWidget *parent = 0);
        virtual QSize sizeHint() const;

    protected:
	QImage img;
	QImage scaledImg;
	
	virtual void paintEvent(QPaintEvent *paintEvent);
	virtual void mouseReleaseEvent ( QMouseEvent * event );
    
    public slots:
	void testForAliveChild();
	void updatePosition();
};