library(readr)
library(reshape2)
library(ggplot2)

data = read_csv2('B01L21_unpack_matrix.csv', 
                 col_types = "cccciii",
                 col_names = c("data_id", "set_id", "origin_id", "dest_id", "origin_gid", "dest_gid", "dist"),
                 na = c("NA"))

ggplot(data, aes(x = origin_id, y = dest_id, fill = dist)) + geom_tile()
