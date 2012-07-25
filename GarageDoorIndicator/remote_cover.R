pdf("remote_cover.pdf", height=5, width=2.5)
par(mar=rep(0.1, 4))
plot(0, 0, type="n", xaxt="n", yaxt="n", xlab="", ylab="",
     xlim=c(0, 250), ylim=c(0,500), xaxs="i", yaxs="i")
u <- par("usr")
midX <- mean(u[1:2])
midY <- mean(u[3:4])
Y <- seq(u[4], u[3], length=12)
X <- seq(u[1], u[2], length=17)
points(midX, Y[10], cex=4)
points(midX, Y[10], cex=0.5, pch=16)

points(X[c(6, 12)], rep(Y[3], 2), cex=4)
points(X[c(6, 12)], rep(Y[5], 2), cex=4)
points(X[c(6, 12)], rep(Y[3], 2), cex=0.5, pch=16)
points(X[c(6, 12)], rep(Y[5], 2), cex=0.5, pch=16)

dev.off()
