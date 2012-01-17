library(Matrix)
library(arules)
library(arulesSequences)

sequence_data <- read_baskets(/Users/Michael/R/x86_64-apple-darwin11.2.0-library/2.14/arulesSequences/misc/zaki.txt, info = c("sequenceID","eventID","SIZE"))
as(sequence_data, "data.frame")

results <- cspade(sequence_data, parameter = list(support = 0.4), control = list(verbose = TRUE))