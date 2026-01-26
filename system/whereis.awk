BEGIN \
{
    where = 0;
    last = -1;
}

{
    if (NR == last) exit(0);
    printf "%04d %s\n",where,$0;
    prev_where = where;
    where += length($0) + 1;
    if (prev_where <= pos && pos < where) {
        printf "%*s^\n", pos - prev_where + 4, "";
        last = NR + 3;
    }
}
