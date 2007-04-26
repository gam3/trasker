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

if ($cgi->path_info eq '/calendar.ics') {

    print <<EOP;

BEGIN:VCALENDAR
PRODID:-//K Desktop Environment//NONSGML KOrganizer 3.4.2//EN
VERSION:2.0

EOP

    my $projects = TTDB::Times->new();
    for my $project ($projects->entries()) {
	my $id = $project->id();
	my $uid = "harpo.gam3.net:" . $id;

	my $start = $project->ical_start;
	my $end = $project->ical_end;
	print <<EOP;

BEGIN:VEVENT
DTSTART:$start
DTEND:$end
DCREATED:20060415T152912Z
UID:$uid
SEQUENCE:0
LAST-MODIFIED:20060415T152912Z
X-ORGANIZER:MAILTO:gam3\@gam3.net
SUMMARY:Test entry
CLASS:PUBLIC
PRIORITY:5
TRANSP:0
END:VEVENT
EOP
    }

print <<EOP;

END:VCALENDAR

EOP
exit(0);

}
use Data::Dumper;
eval {
    print "<h1>User 1</h1>\n";
    print "<h2>Week " . $cgi->param('week') . "</h1>\n";

    require TTDB::UserProject;
    require TTDB::Notes;

    my $up = TTDB::UserProject->new(user => TTDB::User->get(id => 1), project => TTDB::Project->get(id => 1));

    my $p = $up->project;
    my $u = $up->user;

    my $year = 2007;
    my $week = $cgi->param('week');
    require Date::Calc;
    my $date = Date::Calc::MySQL->new( Date::Calc::Monday_of_Week($week, $year) );
    my $end_date = $date + 7;

    while ($date < $end_date) {
        print "Date ", $date->strftime("%Y %B %d %A"), "<br/>\n";

        for my $time ($u->day(date => $date, pids => [ $p->all_ids ])) {
            print "<br/>\n";
            print $time->elapsed, "<br/>\n";
        }

        for my $note ( TTDB::Notes->new( date => $date, project_ids => [ $p->all_ids ], )->entries ) {
            my $name = '';
            if (my $x = $note->project) {
		$name = $x->name;
            }
            print "# Note " . $note->time . " - [$name] - " . substr($note->text, 0, 140), "<br/>\n";
        }
	print "<br/>\n";
	print "<br/>\n";

        $date++;
    }
};
if ($@) {
    print $@;
}

__END__
