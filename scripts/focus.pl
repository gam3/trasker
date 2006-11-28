#!/usr/bin/perl

open IN, "</tmp/focus.log";

my $now = time;
while (<IN>) {
    chomp;
    @time = split('\s');
    @data = split('\.', $time[3]);
    my $len = $time[1] - $time[0];
    print scalar localtime($time[0]), " $len ", $time[3], ' ', $data[1], "\n";
}

close IN;

