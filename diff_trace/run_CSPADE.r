library(Matrix)
library(arules)
library(arulesSequences)

sequence_data <- read_baskets("test.txt", info = c("sequenceID","eventID","SIZE"))
as(sequence_data, "data.frame")

results <- cspade(sequence_data, parameter = list(support = 0.4), control = list(verbose = TRUE))