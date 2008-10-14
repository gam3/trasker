#!/usr/bin/perl

use lib '/usr/src/gam3/tasker/perl/lib/';

use strict;
use CGI;

eval {
    require TTDB::Projects;
    require TTDB::Users;
    require TTDB::Project;
    require TTDB::User;
    require TTDB::Times;
};
use TTDB::User;

use Date::Calc::MySQL;

use TTDB::DBI qw (dbi_setup get_dbh);

dbi_setup(
    database => 'tasker',
    user => 'gam3',
    password => 'awnmwad',
);

our $cgi;

BEGIN {
    $cgi = new CGI;
    if ($cgi->path_info eq '/calendar.ics') {
	print "content-type: text/plain\n\n";
    } else {
	print "content-type: text/html\n\n";
    }
}

my $dbh = get_dbh();

my $st = $dbh->prepare(<<SQL);
select * from timeslice where start_time <= date(?) + interval '1 day' and end_time >= date(?) order by start_time;
SQL

my $d = Date::Calc::MySQL->new($cgi->param('date')) || Date::Calc::MySQL->today;
my $user_name = $cgi->param('user') || 'gam3';

$st->execute($d, $d);

print '<h1>' . $d->format("%D %M %Y") . '</h1>';

while (my $x = $st->fetchrow_hashref()) {
    my $st = Date::Calc::MySQL->new($x->{start_time});
    my $et = Date::Calc::MySQL->new($x->{end_time});
    my $dt = Date::Calc::MySQL->new($x->{elapsed});
}

print "<table>\n";

my $user = TTDB::User->get(user => $user_name);

for my $project ($user->projects) {
eval {
    next unless $project->get_time->as_hours + $project->get_alltime->as_hours;
}; print $@;

    print $project->get_time->as_hours, "</br>";

    print $project->time();
    print $project->alltime();
    print ' ', $project->name, ' ', $project->depth;
    print "</br>";
}

eval {
    for my $time ($user->get_timeslices_for_day(date => Date::Calc::MySQL->new($d))) {
	print "<tr>\n";
	printf("<td>%s</td><td>%s</td><td>%s</td><td>%s</td><th>%s</th>\n", $time->id, $time->project->name, $time->start_time, $time->elapsed, $time );
	print "</tr>\n";
    }
};
print $@;

print "</table>\n";

__END__
