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

if ($cgi->param('show') eq 'users') {
    my $users;
    print "<h1>Users</h1>\n";
    print "<table>\n";
    eval {
	$users = TTDB::Users->new();
	for my $user ($users->entries()) {
	    print " <tr>\n";
	    print "  <td>\n";
	    print $user->id();
	    print "  </td>\n";
	    print "  <td>\n";
	    my $id = $user->id();
	    print qq(<a href="?user=$id">), $user->name(), "</a></td><td>";
	    print "  </td>\n";
	    print "  <td>\n";
	    print $user->fullname();
	    print "  </td>\n";
	    print "  <td>\n";
	    eval {
		print $user->current_project()->id();
	    }; if ($@) {
	        print "fail";
	    }
	    print "  </td>\n";
	    print "  <td>\n";
	    print $user->userid();
	    print "  </td>\n";
	    print " </tr>\n"
	}
    };
    print $@;
} elsif ($cgi->param('show') eq 'projects') {
    my $projects;
    print "<h1>Projects</h1>\n";
    eval {
	$projects = TTDB::Projects->new();
	print "<table>";
	for my $project ($projects->entries()) {
	    my $id = $project->id();
	    print "<tr><td>\n";
	    print qq(<a href="?project=$id">), $project->name(), "</a></td><td>";
	    print $project->longname(), "</td>\n";
	    print "</tr>\n";
	}
	print "</table>";
    };
    print "xxx: ", $@ if $@;
} elsif ($cgi->param('show') eq 'times') {
    my $projects;
    print "<h1>Times</h1>\n";
    eval {
	$projects = TTDB::Times->new();
	print "<table>";
	for my $project ($projects->entries()) {
	    my $id = $project->id();
	    print "<tr><td>\n";
	    print qq(<a href="?time=$id">), $project->name(), "</a></td><td>";
	    print $project->longname(), "</td>\n";
	    print "<td>";
	    print join(' ', keys %{$project->{data}});
	    print "</td>";
	    print "<td>";
	    print $project->{data}->{name};
	    print "</td>";
	    print "</tr>\n";
	}
	print "</table>";
    };
    print $@;
} elsif ($cgi->param('show') eq 'time') {
    print "<h1>Time</h1>\n";

} elsif ($cgi->param('show')) {
    print "<h1>Unknown</h1>\n";
} elsif (my $pid = $cgi->param('project')) {
    my $project;
    eval {
	$project = TTDB::Project->get(id => $pid);
    };
    print $@;
    my $name = $project->longname();
    print "<h1>Project $pid -- $name</h1>\n";
    print qq(<form method="get">\n);
    print qq( <input type="hidden" name="project" value="$pid"/></br>\n);
    print qq(Parent project: <select name="parent_project">);
    print qq(<option value="0">-Root-</option>\n);
    my $projects = TTDB::Projects->new();
    for my $pproject ($projects->entries()) {
        next if ($project->id() == $pproject->id());
	my $x= "";
	$x .= sprintf(qq( value="%s"), $pproject->id());
        if ($project->parent && $project->parent->id() == $pproject->id()) {
	    $x .= " selected";
	}
        print "<option$x>", $pproject->longname, "</option>\n";
    }
    print "</select></br>";
    print qq(Name: <input name="name" value="), $project->name, qq("/></br>\n);
    if ($project->is_task) {
	print qq(<input name="task" type="checkbox" checked/> is_task</br>);
    } else {
	print qq(<input name="task" type="checkbox"/> is_task</br>);
    }
    if ($project->hidden) {
	print qq(<input name="hidden" type="checkbox" checked/> hidden</br>);
    } else {
	print qq(<input name="hidden" type="checkbox"/> hidden</br>);
    }
    print qq(<input type="submit"/>);
    print "</br>";
    print qq(<textarea name="description">);
    print $project->description();
    print qq(</textarea>);
    print "</form></br>";
} elsif (my $uid = $cgi->param('user')) {
    my $user;
    print "<h1>User $uid</h1>\n";
    print "<pre>";
    eval {
	$user = TTDB::User->get(id => $uid);
    };
    print $@;

    print $user->fullname();
    print "<table>";
    for my $project ($user->projects()) {
         print "<tr><td>";
         print $project->project()->longname();
         print "</td></tr>";
    }
    print "</table>";
} else {
    my $projects;
    my $users;
    print qq(<h1><a href="?show=projects">Projects</a> x <a href="?show=users">Users</a></h1>\n);
    eval {
	$projects = TTDB::Projects->new();
	$users = TTDB::Users->new();
	print qq(<form name="main">\n);
	print "<table border=1>";
	print "<tr>";
	print "<th rowspan=\"2\">Projects</th>\n";
	print qq(<th colspan="4" rowspan="1">Users</th>\n);
	print "</tr>";
	print "<tr>";
	for my $user ($users->entries()) {
	    my $id = $user->id();
	    print qq(<th><a href="?user=$id">), $user->name(), "</a></th>\n";
	}
	print "</tr>";
	for my $project ($projects->entries()) {
	    print "<tr>";
	    my $id = $project->id();
	    print "<td>\n";
	    print qq(<a href="?project=$id">), $project->longname(), "</a>";

	    print "</td>\n";
	    for my $user ($users->entries()) {
	        print("<td>");
		if ($project->is_task) {
		    if ($user->id == $project->is_task) {
			printf qq(<input name="project_%d" value="user_%d" type="radio" checked=checked />\n), $project->id, $user->id;
		    } else {
			printf qq(<input name="project_%d" value="user_%d" type="radio">\n), $project->id, $user->id;
		    }
		} else {
		    if ($user->has_project(id => $project->id)) {
			printf qq(<input name="user_%d_project_%d" type="checkbox" checked="checked" />\n), $user->id, $project->id;
		    } else {
			printf qq(<input name="user_%d_project_%d" type="checkbox"/>\n), $user->id, $project->id;
		    }
		}
	        print("</td>\n");
	    }
	    print "</tr>\n";
	}
	print "</table>";
	print qq(<input type="submit"/>\n);
	print qq(</form>\n);
    };
    print $@;
}
__END__
