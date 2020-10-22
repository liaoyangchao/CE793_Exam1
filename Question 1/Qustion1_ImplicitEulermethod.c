#include<stdio.h>
#define N 40

void modeuler(float(*f)(float,float),float t0,float y0,float theta0,float tn,int n)
{
    int i;
    float yp, yc, thetap, thetac, t = t0, y = y0, theta = theta0, h = (tn-t0)/n;
    printf("Question 1");
    printf("t[0]=%f\ttheta[0]=%f\n",t,theta);
    for(i = 1;i <= n;i++)
    {
        yp = y+h*(*f)(t,theta);
		thetap = (theta+h*(*f)(t,y))/(1+h*h);
        t = t0+i*h;     
        printf("t[%d]=%f\ttheta[%d]=%f\n",i,t,i,theta);
    }
}

float f1(float t, float theta)
{
    return (-9.81/0.6*theta);
}

void main()
{
    float tn=6,t0=0,y0=0,theta0=10;
    modeuler(f1,t0,y0,theta0,tn,N);
}