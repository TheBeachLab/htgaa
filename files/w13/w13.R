# GENERAL
require(tikzDevice)
# CASE 2HJJ
setwd("~/Downloads/w13/homework/structure_prediction/2HJJ/")
case2hjj <- read.table("score.fsc", header = TRUE)
tikz('case2hjj.tex',standAlone = TRUE, width = 5,height = 5)
plot (case2hjj$rms, case2hjj$score, main="Case 2HJJ (100 models)\nEnergy vs rms", xlab="rms (Angstrom)", ylab="Energy", pch=3)
dev.off()
tools::texi2dvi('case2hjj.tex',pdf=T)
# CASE 1WHZ
setwd("~/Downloads/w13/homework/structure_prediction/1WHZ/")
case1whz <- read.table("score.fsc", header = TRUE)
tikz('case1whz.tex',standAlone = TRUE, width = 5,height = 5)
plot (case1whz$rms, case1whz$score, main="Case 1WHZ (265 models)\nEnergy vs rms", xlab="rms (Angstrom)", ylab="Energy", pch=3)
dev.off()
tools::texi2dvi('case1whz.tex',pdf=T)
