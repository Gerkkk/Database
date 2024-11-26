# Differences from the task

1) Only one version of insert is implemented - the one that requires all the columns to be mentioned in the order in which they exist in database;
2) In Create Table statement we use () instead of {} when describing attributes of the column. F.e. (autoincrement, key) instead of {autoincrement, key};
3) (2) is needed since we use {} to show subquery (however, it is not yet implemented).