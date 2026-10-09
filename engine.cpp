#include<bits/stdc++.h>
using namespace std;
#define F ios_base::sync_with_stdio(0);cin.tie(0);
#define I 1e18
#define pb push_back
#define mk make_pair
typedef pair<double,int> pdi;
struct E{int v;double d;bool tc,ia,exp,fl,stp,stg,bp,cr,lit,in,fw,al;double aqi;};
map<string,int> N;map<int,string> R;map<int,pair<double,double>> C;vector<vector<E>> A;int n=0;
int G(string s){if(!N.count(s)){N[s]=n;R[n]=s;n++;}return N[s];}
void AE(string u,string v,double d,bool tc,bool ia,bool exp,bool fl,bool stp,bool stg,bool bp,bool cr,bool lit,bool in,bool fw,bool al,double aqi,double u_lat,double u_lon,double v_lat,double v_lon){
int x=G(u),y=G(v);C[x]={u_lat,u_lon};C[y]={v_lat,v_lon};
A[x].pb({y,d,tc,ia,exp,fl,stp,stg,bp,cr,lit,in,fw,al,aqi});
A[y].pb({x,d,tc,ia,exp,fl,stp,stg,bp,cr,lit,in,fw,al,aqi});
}
int main(int argc,char** argv){F
if(argc<8)return 0;
string S=argv[1],D=argv[2],P=argv[3],W=argv[4],WC=argv[5];double AQ=stod(argv[6]);int H=stoi(argv[7]);
A.assign(100,vector<E>());
AE("RC","AIIMS",8000,0,0,1,0,0,0,0,1,1,0,0,0,150,28.6328,77.2197,28.5685,77.2066);
AE("AIIMS","HAZ",4000,1,0,0,1,0,0,0,1,1,0,0,0,80,28.5685,77.2066,28.5430,77.2057);
AE("RC","PAT",1500,1,1,0,0,0,0,0,1,1,0,0,0,50,28.6328,77.2197,28.6219,77.2139);
AE("PAT","CH",1000,0,0,1,0,1,0,1,0,1,0,1,0,200,28.6219,77.2139,28.6143,77.2116);
AE("CH","UDC",1200,1,0,0,0,0,0,0,1,0,0,0,0,90,28.6143,77.2116,28.6094,77.2128);
AE("UDC","LKO",1500,0,0,1,1,0,0,0,1,1,0,0,0,110,28.6094,77.2128,28.6041,77.2084);
AE("LKO","INMA",3000,1,0,0,0,0,0,0,1,1,0,0,0,60,28.6041,77.2084,28.5741,77.2093);
AE("INMA","AIIMS",1000,0,0,1,0,0,1,0,0,1,0,0,1,180,28.5741,77.2093,28.5685,77.2066);
AE("RC","ANV",12000,0,0,1,0,0,0,0,1,1,0,0,0,250,28.6328,77.2197,28.6468,77.3161);
AE("ANV","KW",9000,1,0,0,0,0,0,0,1,1,0,0,0,140,28.6468,77.3161,28.6675,77.2281);
AE("KW","CH",8500,0,1,0,1,0,0,0,1,1,0,0,0,100,28.6675,77.2281,28.6143,77.2116);
if(!N.count(S)||!N.count(D)){cout<<"{\"error\":\"invalid\"}";return 0;}
int s=N[S],d=N[D];
vector<double> D_V(n,I);vector<int> P_V(n,-1);
priority_queue<pdi,vector<pdi>,greater<pdi>> Q;
D_V[s]=0;Q.push(mk(0,s));
while(!Q.empty()){
double c=Q.top().first;int u=Q.top().second;Q.pop();
if(c>D_V[u])continue;
if(u==d)break;
for(auto&e:A[u]){
double w=e.d/5.0;
if(W=="extreme_heat"){if(e.tc||e.ia)w*=0.5;if(e.exp)w*=2.5;}
if(W=="heavy_rain"){if(e.fl)continue;}
if(AQ>300){w+=e.aqi*0.1;}
if(WC=="true"){if(e.stp||e.stg||e.bp||!e.cr)continue;}
if(P=="auto"){if(e.fw||e.stp||e.al)continue;}
if(P=="delivery"){if(e.in)continue;}
if(P=="student"||P=="pedestrian"){if(H>=22&&!e.lit)w*=2.0;}
if(D_V[u]+w<D_V[e.v]){D_V[e.v]=D_V[u]+w;P_V[e.v]=u;Q.push(mk(D_V[e.v],e.v));}
}
}
if(D_V[d]==I){cout<<"{\"path\":[]}";return 0;}
vector<string> p;int c=d;while(c!=-1){p.pb(R[c]);c=P_V[c];}
reverse(p.begin(),p.end());
cout<<"{\"path\":[";
for(size_t i=0;i<p.size();i++){cout<<"\""<<p[i]<<"\""<<(i+1==p.size()?"":",");}
cout<<"],\"cost\":"<<D_V[d]<<",\"eta_mins\":"<<(int)(D_V[d])<<",\"total_dist_m\":1000}";
return 0;
}
