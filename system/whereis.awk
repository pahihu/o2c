BEGIN \
{
    where = 0;
    last = -1;
}

{
    if (NR == last) exit(0);
    printf "%06d %s\n",where,$0;
    prev_where = where;
    where += length($0) + 1;
    if (prev_where <= pos && pos < where) {
        printf "...... ";
        for (i = 0; i < pos - prev_where; i++)
            printf "-";
        printf "^\n";
        last = NR + 3;
    }
}
