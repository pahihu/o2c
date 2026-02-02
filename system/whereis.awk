BEGIN \
{
    where = 0;
    last = -1;
    print;
}

{
    if (NR == last) exit(0);
    prev_where = where;
    if (pos - prev_where < 200)
        printf "%06d %s\n",where,$0;
    where += length($0) + 1;
    if (prev_where <= pos && pos < where) {
        printf "...... ";
        for (i = 0; i < pos - prev_where; i++)
            printf "-";
        printf "^\n";
        printf "...... line %d\n", NR;
        last = NR + 3;
    }
}
