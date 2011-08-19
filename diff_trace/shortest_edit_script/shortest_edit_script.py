#!/usr/bin/python

import math

def LCS(A,N,B,M) :
#    print "BEGIN LCS"

    if N>0 and M>0 :
        D, x, y, u, v = middleSnake(A, N, B, M)

#         print "D: " + str(D) + " x: " + str(x) + " y: " + str(y) + " u: " + str(u) + " v: " + str(v)

        if D > 1 :
#            print "u: " + str(u) + " v: " + str(v)
            LCS(A[0:u],u,B[0:v],v)
            print A[u:x]
            LCS(A[x:N],N-x,B[y:M],(M-y))
        elif M > N :
            print A[0:N]
        else :
            print B[0:M]

#    print "END LCS"

    return;

def middleSnake(A, N, B, M) :

#    print "M: " + str(M) + " N: " + str(N)
    delta = N-M
    V1 = [-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1]
    V2 = [N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N]

    for D in range(0,int(math.ceil((M+N)/2))+1,1) :
#        print "Forward D:" + str(D)
        for k in range(-D,D+1,2) :
#            print "k:" + str(k)
            if k==-D or (k!=D and V1[k-1+N]<V1[k+1+N]) :
                x=V1[k+1+N]
            else :
                x=V1[k-1+N]+1

            y=x-k

            print "N:" + str(N) + " M:" + str(M)
            print "Length:" + str(len(A)) + " Length:" + str(len(B))
            print "x: " + str(x) + " y: " + str(y)
            while x<(N-1) and y<(M-1) and A[x+1]==B[y+1] :
                x+=1
                y+=1

#            print "x: " + str(x) + " y: " + str(y)
            V1[k+N]=x

            if delta % 2 == 1 :
                if k in range(delta-(D-1), delta+(D-1)+1) :
                    if x >= V2[k+N]:
#                        print str(V2[N])
                        return (2 * D-1), x, y, V2[k+N], V2[k+N]-k

#        print "Backward D:" + str(D)
        for k in range(-D+delta,D+1+delta,2) :
#            print "k:" + str(k)
            if k==D or (k!=-D and V2[k-1+N]<V2[k+1+N]) :
                x=V2[k-1+N]
            else :
                x=V2[k+1+N]-1

            y=x-k

#            print "N:" + str(N) + " M:" + str(M)
#            print "Length:" + str(len(A)) + " Length:" + str(len(B))
#            print "x: " + str(x) + " y: " + str(y)
            while x>0 and y>0 and x<=N and y<=M and A[x-1]==B[y-1] :
                x-=1
                y-=1

            V2[k+N]=x

            if delta % 2 == 0 :
                if k in range(-D, D+1) :
                    if x <= V1[k+N]:
#            print str(V2[N])
                        return (2 * D), V1[k+N], V1[k+N]-k, x, y
    return 0,0,0,0,0

#A = ["a", "b", "c", "d"]
#B = ["a", "c", "c", "d"]

#LCS(A, 4, B, 4)

print

#A = ["a", "b", "c", "d", "e"]
#B = ["a", "c", "c", "d"]

#LCS(A, 5, B, 4)

#print

A = ["a", "b", "c", "d", "e", "f"]
B = ["a", "c", "c", "d"]

LCS(A, 6, B, 4)
