# コマンド入力判定
書式  
[Remap]  
--------必須  
D = *int*  
F = *int*  
B = *int*  
U = *int*  
--------任意  
*buttonname* = *int*  
...  

[Command]    
command = *string*  
name  = *<seq>*  
time = *int*  
  
***
\<seq> := \<sametime> {, \<sametime>}  
\<sametime> := \<terminal> {+ \<terminal>}  
\<terminal> := buttonname \| \<attr> buttonname  
\<attr> := /  |  >  |  ~
***


