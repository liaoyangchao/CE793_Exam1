#include<stdio.h>
#define N 40

void modeuler(float(*f)(float,float),float t0,float y0,float theta0,float tn,int n)
{
    int i;
    float y1, y2, y3, y4, k1, k2, k3, k4, yc, thetap, thetac, t = t0, y = y0, theta = theta0, h = (tn-t0)/n;
    printf("Question 1");
    printf("t[0]=%f\ttheta[0]=%f\n",t,theta);
       
    for(i = 2;i <= n;i++)
    {
        k1 = (*f)(t,y);
        y1 = (*f)(t,y)+k1*0.5*h;
        k2 = (*f)(t,y1);
        y2 = (*f)(t,y1)+k2*0.5*h;
        k3 = (*f)(t,y2);
        y3 = (*f)(t,y2)+k3*0.5*h;
        k4 = (*f)(t,y3);
        y4 = (*f)(t,y3);
        
        thetac = theta + (k1/6+k2/3+k3/3+k4/6)*h;
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