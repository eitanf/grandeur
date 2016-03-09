library(ggplot2)
library(reshape2)

data <- read.csv('gamestats.csv', header = TRUE)

# Percentage of games lost (as opposed to won/tied):
qplot(data$Minimax_level, data$loss_rate) + geom_line()

# Mean no. of rounds per game:
qplot(data$Minimax_level, data$rounds_per_game) + geom_line()

# Mean difference in points between minimax and greedy:
qplot(data$Minimax_level, data$point_diff) + geom_line()

# Mean no. of nobles won by player 0 (minimax) and player 1 (greedy)
ggplot(data, aes(x = Minimax_level)) + 
  geom_line(aes(y = nobles_0, colour = "Nobles P0")) + 
  geom_line(aes(y = nobles_1, colour = "Nobles P1"))

# Ratio of game move types for minimax:
df <- melt(data[,c(1,5:8)], id.var = "Minimax_level")
ggplot(df, aes(x = Minimax_level, y=value, fill=variable)) + geom_bar(stat = "identity")

# Ratio of type (deck) of cards bought by minimax:
df <- melt(data[,c(1,12:14)], id.var = "Minimax_level")
ggplot(df, aes(x = Minimax_level, y=value, fill=variable)) + geom_bar(stat = "identity", position = "dodge")
