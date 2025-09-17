library(ggplot2)
library(reshape2)
library(readr)

files = list.files(pattern = "times_.*\\.csv")

data = lapply(files, function(f) {
  
  title = sub("\\.csv", "", f)
  
  distr = read_delim(f, ";", escape_double = FALSE, col_names = c('time','freq'))
  
  obs = as.vector(rep(distr$time, distr$freq))
  
  plot = ggplot(data.frame(times = obs), aes(x = times, fill = ..count..)) +
    geom_histogram(bins = 100) +
    #geom_line() +
    labs(title = title, x = "Travel time (s)", y = "Frequency")
  
  ggsave(sprintf("%s.png", title), plot)
  return(data.frame(distr, route = title))
})

data_m = melt(data, id.vars = c('route'), measure.vars = c('time', 'freq'))





# plot = ggplot(distr, aes(x = time, y = prob)) + 
#   #geom_bar(stat = 'identity') +
#   geom_line() + 
#   labs(title = title, x = "Travel time (s)", y = "Frequency")
# 
# ggsave(sprintf("%s.png", title), plot)