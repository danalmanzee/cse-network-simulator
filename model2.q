A=FIFO()
B=ServerExp(0.3)
C=FIFO()
D=ServerExp(0.6)
E=FIFO()
F=ServerNormal(2,0.5)
G=FIFO()
H=ServerCst(1)
X=Exit
K=Poisson(4.0)
L=Poisson(2.0)
I=Dispatch([(A,0.2),(E,0.4),(G,0.4)])
J=Dispatch([(X,0.8),(C,0.2)])
A->B
C->D
E->F
G->H
K->A
L->C
B->I
D->G
F->X
H->J