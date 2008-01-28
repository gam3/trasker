#!/usr/bin/perl

use lib '/usr/src/gam3/tasker/perl/lib/';

use strict;
use CGI;

eval {
    require TTDB::Projects;
    require TTDB::Users;
    require TTDB::Project;
    require TTDB::User;
    require TTDB::UserProject;
    require TTDB::Times;
};

use TTDB::DBI qw (dbi_setup);

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

if ($cgi->param('year')) {

}
if ($cgi->param('week')) {

}

if ($cgi->param('project')) {

}

my $project = eval {
    TTDB::Project->get(name => $cgi->param('project'));
};
if ($@) {
    print $@;
    exit;
}

my @users = eval {
    $project->users;
};
if ($@) {
    print $@;
    exit;
}

use Date::Calc::Object qw (Monday_of_Week Week_of_Year Add_Delta_Days Time_to_Date Day_of_Week);
use Date::Calc::MySQL;

my $year = Date::Calc::MySQL->today->year;
my $week = $cgi->param('week');

my $date = Date::Calc::MySQL->new(Monday_of_Week($week, $year));

print $project->name . ' ' . $project->description . "<br/>\n";
print "Week of: " . $date->strftime("%D") . " ($year-w$week)" . "<br/>\n";
print "<br/>\n";

eval {
    for (my $d = $date; $d < $date + 7; $d++) {
	for my $user ($project->users->entries) {
	    my $timespan_all = $user->day(date => $d, all => 1);
            next unless $timespan_all->duration;
print $d->strftime("%D"), "<br/>\n";
	    my $timespan = $user->day(date => $d);
print 'Misc. ', $timespan->duration, "<br/>\n";
	    for my $child ($user->children) {
		my $timespan = $child->day(date => $d);
		my $time = $timespan->duration;
		next unless $time;
		print $child->user->name, " ";
		print $child->name, " ";
		print $child->rate, ' ';
		print $time . "</br>";
	    }
#print join("\n", map({ $_->text . "<br/>"} $user->get_notes()));
	    print "<br/>\n";
	}
    }
};
if ($@) {
    print 'eval (' . $@ . ')';
}

__END__
