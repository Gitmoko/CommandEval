# コマンド入力判定
## 書式  
[Remap]  
--------必須。D:下,F:前,B:後ろ,U:上  
D = *int*  
F = *int*  
B = *int*  
U = *int*  
--------任意  
*buttonname* = *int*  
...  

[Command]    
command = "*string*"  
name  = \<seq>  
time = *int*  
  
***
\<seq> := \<sametime> {, \<sametime>}  
\<sametime> := \<terminal> {+ \<terminal>}  
\<terminal> := \<press> \| \<attr> \<press>  
\<press> := *buttonname* |*int* *buttonname*  
\<attr> := /  |  >  |  ~  

***

## Sample
[ Command ]  
name = "Hadou"  
command   = /D,DF,F,a  
time = 60  
/は押しっぱなしに反応。斜めはDorU,BorFの順に記述し表す。   

[ Command ]  
name = "ab"  
command = a+b  
time = 1  
+は同時押しに反応  

[ Command ]   
name = "tame"  
command   = ~30a,b+c  
time = 10  
aを30フレーム押してからはなし、bとcの同時押し  

[ Command ]   
name = "dash"  
command   = F,\>F  
time = 10  
\>は正確な方向を表す。Fは(斜め前->前)という入力でも反応するが、>を付けると前方向の成分が残っている状態からはFは反応しなくなる。
