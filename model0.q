B=Poisson(4.0)
A=FIFO()
C=ServerExp(0.3)
X=Exit
B->A
A->C
C->X
