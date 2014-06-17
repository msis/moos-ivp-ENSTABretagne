#include "observer.h"
//#include "Unit1.h"


//extern TForm1 *Form1;


//---------------------------------------------------------------------------
observer::observer()	{   

	Lu1.clear(); Lu2.clear(); Ld.clear(); Lalpha.clear(); LXhat.clear(); Xhat=box(4);
	chenillex.clear();chenilley.clear();chenilletheta.clear();chenillealpha.clear();chenilled.clear();
	char line[256];
	FILE *fp = fopen("observer.txt","r");
	if (!fp)	{
		fprintf(stderr, "Error opening the file (observer.txt)\n");
	}
	double prob_outliers=0;
	fgets2(fp,line); sscanf(line,"%lf",&prob_outliers);
	fgets2(fp,line); sscanf(line,"%d",&N_Bissect_0);
	fgets2(fp,line); sscanf(line,"%d",&N_Bissect_k);
	fgets2(fp,line); sscanf(line,"%lf",&dt);
	fgets2(fp,line); sscanf(line,"%lf",&ws);
	fgets2(fp,line); sscanf(line,"%lf",&coef_friction);
	fgets2(fp,line); sscanf(line,"%lf",&coef_puissance_moteur);
	fgets2(fp,line); sscanf(line,"%lf",&coef_ecart_moteur);
	fgets2(fp,line); sscanf(line,"%lf",&xmin);
	fgets2(fp,line); sscanf(line,"%lf",&xmax);
	fgets2(fp,line); sscanf(line,"%lf",&ymin);
	fgets2(fp,line); sscanf(line,"%lf",&ymax);
	// fgets2(fp,line); sscanf(line,"%lf",&thetamin);
	// fgets2(fp,line); sscanf(line,"%lf",&thetamax);
	fgets2(fp,line); sscanf(line,"%lf",&vmax0); // a l'initialisation, la vitesse est faible
	fgets2(fp,line); sscanf(line,"%lf",&vmax);  // vitesse max du robot
	fgets2(fp,line); sscanf(line,"%d",&N_cercles);
	N_window=(int)(floor(2*3.14/(dt*ws)));  // taille de l'horizon des mesures : correspond au nombre de mesures sonar par tour de sonar
	N_outliers=(int)(prob_outliers*N_window); // nombre de outliers autorisés par tour de sonar
	for (int i=0;i<N_cercles;i++)	{   
		double x,y,r;
		fgets2(fp,line); sscanf(line,"%lf%lf%lf",&x,&y,&r);
		cercles_x.push_back(x); cercles_y.push_back(y); cercles_r.push_back(r);
	}
	while (fgets2(fp,line) != NULL)	{   
		double xa,ya,xb,yb;
		sscanf(line,"%lf%lf%lf%lf",&xa,&ya,&xb,&yb);
		murs_xa.push_back(xa);  murs_ya.push_back(ya);
		murs_xb.push_back(xb);  murs_yb.push_back(yb);
		N_murs=(int)(murs_xa.size());
	}
	fclose(fp);
	phase=0;
	// Xprior0 est la boite initiale à l'instant initial ; la vitesse est quasi-nulle
	Xprior0=box(4);    
	Xprior0[1]=interval(xmin,xmax);		Xprior0[2]=interval(ymin,ymax);
	Xprior0[3]=interval(-PI,PI);		Xprior0[4]=interval(0,vmax0);
	Xhat=Xprior0;
	Xsecours=Xhat;
	// Xpriork est la boite contenant l'état pour tout k ; la vitesse n'est plus négligeable
	Xpriork=box(4);     
	Xpriork[1]=interval(xmin,xmax);		Xpriork[2]=interval(ymin,ymax);
	Xpriork[3]=interval(-1000,1000);	Xpriork[4]=interval(0,vmax);
}
//---------------------------------------------------------------------------
void observer::Incremente(interval& X1,interval& Y1, interval& Theta1,interval& V1, interval& X,interval& Y,interval& Theta,interval& V, double u1,double u2)	{        

	if (phase==0)	{   
		X1=Inter(X,Xprior0[1]);  // La dynamique en phase d'initialisation est immobile
		Y1=Inter(Y,Xprior0[2]);  // elle est différente de celle en mission
		Theta1=Inter(Theta,Xprior0[3]) ;
		V1=Inter(V,Xprior0[4]);
	}
	if (phase==1)	{   
		X1    = X     + dt*Cos(Theta)*V;
		Y1    = Y     + dt*Sin(Theta)*V;
		Theta1= Theta + dt*coef_ecart_moteur*(u2-u1);
		V1    = V     + dt*(coef_puissance_moteur*(u1+u2)-coef_friction*V);
	}
}
//----------------------------------------------------------------------
void observer::Decremente(interval& X1,interval& Y1, interval& Theta1,interval& V1, interval& X,interval& Y,interval& Theta,interval& V,double u1,double u2)	{         

	if (phase==0)	{   
		X=Inter(X1,Xprior0[1]);
		Y=Inter(Y1,Xprior0[2]);
		Theta = Inter(Theta1,Xprior0[3]);
		V=Inter(V1,Xprior0[4]);
	}
	if (phase==1)	{   
		Theta = Theta1-dt*coef_ecart_moteur*(u2-u1);
		V     = (V1-dt*coef_puissance_moteur*(u1+u2))/(1-dt*coef_friction);
		X     = X1-dt*Cos(Theta)*V;
		Y     = Y1-dt*Sin(Theta)*V;
	}
}
//----------------------------------------------------------------------
bool observer::Contract(box& P)	{

	vector<box> L;
	vector<box> Pi(N_window);
	Pi[N_window-1]=P;

	// Dans ce qui suit, on recopie les listes dans des vecteurs pour avoir un accès direct (c'est pas terrible, mais je ne sais pas faire autrement)
	// Je sais qu'il existe une classe qui permet de faire une liste-vecteur, mais je ne sais pas son nom.
	vector<double>   Vu1(N_window);     list<double>::iterator iLu1=Lu1.begin();       for(int i=0;i<N_window;i++)  {Vu1[i]=*iLu1;       iLu1++;};
	vector<double>   Vu2(N_window);     list<double>::iterator iLu2=Lu2.begin();       for(int i=0;i<N_window;i++)  {Vu2[i]=*iLu2;       iLu2++;};
	vector<double>   Valpha(N_window);  list<double>::iterator iLalpha=Lalpha.begin(); for(int i=0;i<N_window;i++)  {Valpha[i]=*iLalpha; iLalpha++;}
	vector<interval> Vd(N_window);      list <interval>::iterator iLd=Ld.begin();      for(int i=0;i<N_window;i++)  {Vd[i]=*iLd;         iLd++;}
	vector<box>      VXhat(N_window-1); list <box>::iterator iLXhat=LXhat.begin();     for(int i=0;i<N_window-1;i++){VXhat[i]=*iLXhat;   iLXhat++;}

	for  (int i=N_window-2;i>=0;i--)	{
		Pi[i]=box(4);
		Decremente(Pi[i+1][1],Pi[i+1][2],Pi[i+1][3],Pi[i+1][4],Pi[i][1],Pi[i][2],Pi[i][3],Pi[i][4],Vu1[i],Vu2[i]);
		Pi[i]=Inter(Pi[i],VXhat[i]);
	}

	for  (int i=0;i<N_window;i++)	{
		interval xi=Pi[i][1];       interval yi=Pi[i][2];
		interval thetai=Pi[i][3];   interval vi=Pi[i][4];
		interval betai=Valpha[i]+thetai;
		interval di=Vd[i];
		CLegOnWallsOrCircles(di,xi,yi,betai,murs_xa,murs_ya,murs_xb,murs_yb,cercles_x,cercles_y,cercles_r);
		Cplus(betai,Valpha[i],thetai,-1);
		interval xip,yip,thetaip,vip;
		for  (int i1=i;i1<N_window-1;i1++)	{     
			Incremente(xip,yip,thetaip,vip,xi,yi,thetai,vi,Vu1[i1],Vu2[i1]);
			xip=Inter(xip,Pi[i1+1][1]);yip=Inter(yip,Pi[i1+1][2]);
			thetaip=Inter(thetaip,Pi[i1+1][3]);vip=Inter(vip,Pi[i1+1][4]);
			xi=xip; yi=yip; thetai=thetaip; vi=vip;
		}
		box Pb(P);
		Pb[1]=xi; Pb[2]=yi; Pb[3]=thetai; Pb[4]=vi;
		L.push_back(Pb);
	}
	vector<box> Lv=L;
	C_q_in(P,N_window-N_outliers,Lv);

	return true;
}
//----------------------------------------------------------------------
box observer::SIVIA(box& X0,box& Psecours)	{
        int NbissectMax = N_Bissect_0;
	if (phase==0) NbissectMax=N_Bissect_0;
	if (phase==1) NbissectMax=N_Bissect_k;
	int Nbissect=0;
	Psecours = box(4);Psecours[1] = 5;Psecours[2] = 5;Psecours[3] = 0;Psecours[4] = 0;
	vector<box> Result;
	list<box> L;
	L.push_back(X0);
	box P;
	//Form1->R1.image->Refresh();
	while (!L.empty())	{
		P=L.front();
		//Form1->R1.DrawCadre(P,clBlue,psSolid,1,2,1);
		L.pop_front();
		bool sortie=false;
		while (!sortie)	{
			box Pold(P);
			Contract(P);
			if (P.IsEmpty()) sortie=true;
			if (decrease(Pold,P)<0.05) sortie=true;
		}
		if (!P.IsEmpty())	{
			if (Nbissect>NbissectMax)	{
				//Form1->R1.DrawBox(P,clRed,clRed,bsSolid,1,2);
				//Form1->R2.image->Refresh();
				Result.push_back(P);
			}
			else	{
				box P1,P2;
				Bisect(P,P1,P2);
                                Psecours=P;
				Nbissect++;
				L.push_back(P1);
				L.push_back(P2);
			}
		}
	}
	box R=Union(Result);
	//Form1->R1.DrawCadre(R,clGreen,psSolid,1,2,1);
	L.clear();

	return R;
}
//----------------------------------------------------------------------
void observer::DrawDiagram()	{

	//Form1->R4.Clean();
	list<interval>::iterator iLd; list<double>::iterator iLalpha;
	iLd=Ld.begin();  iLalpha=Lalpha.begin();
	for  (int i=0;i<N_window;i++)	{
		//Form1->DrawRobot1(Form1->R4,0,0,0, *iLalpha, (*iLd).inf,clRed,2);
		//Form1->DrawRobot1(Form1->R4,0,0,0, *iLalpha, (*iLd).sup,clRed,1);
		iLalpha++; iLd++;
	}
	int chenillesize=(int)(chenillex.size());
	list<double>::iterator ichenillealpha; list<double>::iterator ichenilled;
	ichenillealpha=chenillealpha.begin();  ichenilled=chenilled.begin();
	for  (int i=0;i<chenillesize;i++)	{
		//Form1->DrawRobot1(Form1->R4,0,0,0, *ichenillealpha, *ichenilled,clGreen,2);
		ichenillealpha++; ichenilled++;
	}
	//Form1->R4.image->Refresh();
}

//----------------------------------------------------------------------
void observer::DrawChenille(double xhat,double yhat, double thetahat, double alpha)	{

	//Form1->R3.Clean();
	//Form1->DrawRoom(Form1->R3);
	if ((int)chenillex.size()>N_window)	{
		chenillex.pop_front(); chenilley.pop_front(); chenilletheta.pop_front(); chenillealpha.pop_front(); chenilled.pop_front();  }
	double d1a=DistanceDirSegments(xhat,yhat,alpha+thetahat,murs_xa,murs_ya,murs_xb,murs_yb);
	double d1b=DistanceDirCercles(xhat,yhat,alpha+thetahat,cercles_x,cercles_y,cercles_r);
	double d1=min(d1a,d1b);
	chenillex.push_back(xhat); chenilley.push_back(yhat); chenilletheta.push_back(thetahat); chenillealpha.push_back(alpha);     chenilled.push_back(d1);
	int chenillesize=(int)(chenillex.size());
	list<double>::iterator ichenillex; list<double>::iterator ichenilley; list<double>::iterator ichenilletheta; list<double>::iterator ichenillealpha; list<double>::iterator ichenilled;
	ichenillex=chenillex.begin();      ichenilley=chenilley.begin();      ichenilletheta=chenilletheta.begin();  ichenillealpha=chenillealpha.begin();  ichenilled=chenilled.begin();
	for (int i=0;i<chenillesize;i++)	{
		//Form1->DrawRobot1(Form1->R3,*ichenillex,*ichenilley,*ichenilletheta, *ichenillealpha, *ichenilled,clGreen,1);
		ichenillex++; ichenilley++; ichenilletheta++; ichenillealpha++; ichenilled++;
	}
	//Form1->R3.image->Refresh();
}
//---------------------------------------------------------------------------
bool observer::clock(double u1,double u2,interval d,double alpha, interval boussole)	{

	//  Au cas ou une boussole est accessible, il faut la ligne suivante
	//  Sinon, la ligne suivante est inutile. Donc, dans tous les cas, il vaut mieux la laisser.
	interval sinTheta=Inter(Sin(Xhat[3]),Sin(boussole)); Csin(sinTheta,Xhat[3],-1);   interval cosTheta=Inter(Cos(Xhat[3]),Cos(boussole));      Ccos(cosTheta,Xhat[3],-1);

	Lu1.push_back(u1); Lu2.push_back(u2);  Ld.push_back(d);  Lalpha.push_back(alpha);
        if ((int)Lu1.size()>N_window)	{
		Lu1.pop_front(); Lu2.pop_front(); Ld.pop_front(); Lalpha.pop_front();  }
	if ((int)Lu1.size()==N_window)
		Xhat=SIVIA(Xhat,Xsecours);
	else	{
		if (phase==0) Xhat=Xprior0;
		if (phase==1)
			return(false); // Cette ligne de code ne devrait jamais être atteinte.
	}
	LXhat.push_back(Xhat);
	if ((int)LXhat.size()>N_window-1) {LXhat.pop_front();}
	if (Xhat.IsEmpty())	{  // An inconsistency is detected. The observer is not reliable anymore.
		//Form1->R1.DrawBox(Xpriork,clAqua,clAqua,bsSolid,1,2);
		return false;
	}
	//DrawChenille(Center(Xhat[1]),Center(Xhat[2]),Center(Xhat[3]),alpha);
	//DrawDiagram();

	box Xhatp(4);
	Incremente(Xhatp[1],Xhatp[2],Xhatp[3],Xhatp[4],Xhat[1],Xhat[2],Xhat[3],Xhat[4],u1,u2);
	box StateNoise(4);
	interval Noise=interval(-0.1,0.1);
	StateNoise[1]=dt*Noise; StateNoise[2]=dt*Noise; StateNoise[3]=dt*Noise; StateNoise[4]=dt*Noise;
	Xhatp=Xhatp+StateNoise;
	Xhat=Xhatp;
	if ((Xhat.Width()>5)&(phase==1)) return false; //On ne se localise pas avec une assez bonne précision. On sort.

	return true;
}
//---------------------------------------------------------------------------



