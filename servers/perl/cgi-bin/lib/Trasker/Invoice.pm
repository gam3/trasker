
use strict;
package Tasker::Invoice;

use lib '/home/gam3/src/tasker/perl/lib';


use base 'Tasker';
use TTDB::Project;
use Date::Calc::MySQL;

use Date::Calc::Object qw (Monday_of_Week Week_of_Year Add_Delta_Days Time_to_Date Day_of_Week);

sub parameters
{
    my $self = shift;
    {
        week => 1,
	project => 1,
	user => 1,
    }
}

sub title
{
    my $self = shift;
    my $date = $self->get('date');
    my $project = $self->get('up')->project;
    "Tasker: Invoice for " . $project->name . ' week of ' . $date->strftime('%F');
}

sub init
{
    my $self = shift;

    my $year = Date::Calc::MySQL->today->year;
    my $week = $self->param('week');
    my $project_name = $self->param('project');
    my $user_name = $self->param('user');

    $self->set(year => $year);
    $self->set(week => $week);
    $self->set(date => Date::Calc::MySQL->new(Monday_of_Week($week, $year)));
    if ($user_name) {
	require TTDB::UserProject;
	my $project =  TTDB::Project->get(name => $project_name);
	my $user =  TTDB::User->get(user => $user_name);
	$self->set(up => TTDB::UserProject->new(project => $project, user => $user));
    } else {
	$self->set(project => TTDB::Project->get(name => $project_name, user => $self->get('user')));
    }

    undef;
}

use Data::Dumper;

sub children
{
    my $self = shift;
    my $up = shift;
    my $title = shift;
    my $d = shift;
    my $text;

    my @children = $up->children;

    my $dur = $up->week(date => $d);

    if (scalar @children + $dur->duration->as_hours > 0) {
        if ($dur->duration->as_hours + 0) {
	    $text .= '<tr><td>' . join('</td><td>', $title,
	      sprintf('$%0.2f', $up->rate),
	      $dur->duration->as_hours, 
	      sprintf('$%0.2f', $up->rate * $dur->duration->as_hours)
	    ) . '</td></tr>';
	}

	for my $child (@children) {
	    $text .= $self->children($child, $child->project->name, $d);
	}
    }

    $text;
}

sub children_day
{
    my $self = shift;
    my $up = shift;
    my $title = shift;
    my $d = shift;
    my $text;

    my @children = $up->children;

    my $dur = $up->day(date => $d);

    if (scalar @children + $dur->duration->as_hours > 0) {
        if ($dur->duration->as_hours + 0) {
	    $text .= '<tr><td>' . join('</td><td>', $title,
	      sprintf('$%0.2f', $up->rate),
	      $dur->duration->as_hours, 
	      sprintf('$%0.2f', $up->rate * $dur->duration->as_hours)
	    ) . '</td></tr>';
	}

	for my $child (@children) {
	    $text .= $self->children_day($child, $child->project->name, $d);
	}
    }

    $text;
}

sub html
{
    my $self = shift;

    my $week = $self->get('week');
    my $year = $self->get('year');
    my $date = $self->get('date');

    my $up = $self->get('up');

    my $project = $up->project;

    Date::Calc->date_format(1);

    my $html = $project->name . ' ' . $project->description . "<br/>\n";
    $html .= "Week of: " . $date->strftime("%D") . " ($year-w$week)" . "<br/>\n";
    $html .= "<br/>\n";

    my $total = $up->week(date => $date, all => 1);
    my @notes = $total->get_notes(exclude_type => 99);

    for my $n (@notes) {
	$html .= $n->date . ' ' . $n->text . "<br/>\n";
    }

    my $data = '<table>';
    $data .= "<tr><th>Description</th><th>Rate</th><th>Hours</th><th>Amount</th></tr>\n";
    $data .= $self->children($up, '-Misc', $date);
    $data .= '<tr><td><td><td colspan="2">Total for the week</td><td>' . $total->duration->as_hours . '</td></tr>';
    $data .= '</table>';

    my $by_day = '<table>';
    for (my $d = $date; $d < $date + 7; $d++) {
	$by_day .= " <tr>\n";
	$by_day .= "  <td>\n";
	$by_day .= qq(<a href="/cgi-bin/ts/timeedit?date=) . $d->mysql . qq(">) . $d . qq(</a>\n);
	$by_day .= "  </td>\n";
	$by_day .= "  <td>\n";

	$by_day .= $self->children_day($up, '-Misc', $d);

	$by_day .= "  </td>\n";
	$by_day .= " </tr>\n";
    }
    $by_day .= '</table>';

    return $html . $data . $by_day;
}

1;
