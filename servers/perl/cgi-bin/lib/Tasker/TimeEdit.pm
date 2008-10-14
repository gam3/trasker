
use strict;
package Tasker::TimeEdit;

use lib '/home/gam3/src/tasker/perl/lib';

use base 'Tasker';
use TTDB::Project;
use Date::Calc::MySQL;

use Date::Calc::Object qw (Monday_of_Week Week_of_Year Add_Delta_Days Time_to_Date Day_of_Week);

sub parameters
{
    my $self = shift;
    {
        date => 1,
	project => 1,
    }
}

sub title
{
    my $self = shift;
    my $date = $self->get('date');

    "Tasker: Time edit for " . $date;
}

sub init
{
    my $self = shift;

    my $project_name = $self->param('project');
    my $user_name = $self->param('user');

#    my $project = TTDB::Project->get(name => $project_name);

    my $date = Date::Calc::MySQL->new($self->param('date'));
    $self->set(date => $date);
#    $self->set(project => $project);

    undef;
}

use Data::Dumper;

Date::Calc->date_format(1);

sub html
{
    my $self = shift;

    my $date = $self->get('date') || Date::Calc::MySQL->today();
    my $project = $self->get('project');

    my $ret = '';
    $ret .= $date . "<br/>\n";

    use TTDB::Times;

    my @time = TTDB::Times->get(date => $date);

    $ret .= "<table>\n";

    for my $time (@time) {
        $ret .= "<tr><td>\n";
        $ret .= $time->start_time->strftime('%H:%M:%S') . '</td><td>' . $time->duration->as_hours . ' '
	. $time->project->longname . '<br/>';
        $ret .= "</td></tr>\n";
	for my $note ($time->notes()->entries) {
	    $ret .= "<tr><td><td>\n";
	    $ret .= 'date: ' . $note->date . "<br/>\n";
	    $ret .= 'Note: ' . $note->text . ' ' . $note->project . '<br/>';
	    $ret .= "</td></tr>\n";
	    
	}
    }

    $ret .= "<table>\n";
    $ret;
}

1;
