library(Matrix)
library(arules)
library(arulesSequences)

sequence_data <- read_baskets("out.txt", sep = "[\t]+", info = c("sequenceID", "eventID", "SIZE"))
as(sequence_data, "data.frame")

results <- cspade(sequence_data, parameter = list(support = 0.02, maxsize = 10, maxlen = 10), control = list(verbose = TRUE))

summary(results)

as(results, "data.frame")