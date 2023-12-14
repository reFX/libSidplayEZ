//
// This file contains chip-profiles which allow us to adjust
// certain settings that varied wildly between 6581 chips, even
// made in the same factory on the same day.
//
// This works under the assumption that the authors used the
// same SID chip their entire career.
//
{ "Anthony Lees",		{	.folder = "/MUSICIANS/L/Lees_Anthony/",				.filter = 0.55	} },
{ "Antony Crowther",	{	.folder = "/MUSICIANS/C/Crowther_Antony/",			.filter = 0.6	} },
{ "Ben Daglish",		{	.folder = "/MUSICIANS/D/Daglish_Ben/",				.filter = 0.1,	.exceptions = { { "Last_Ninja", "Anthony Lees" } } } },	// Only Anthony Lees used the filter
{ "Charles Deenen",		{	.folder = "/MUSICIANS/D/Deenen_Charles/",			.filter = 0.0	} },
{ "Chris Huelsbeck",	{	.folder = "/MUSICIANS/H/Huelsbeck_Chris/",			.filter = 0.4	} },
{ "Clever Music",		{	.folder = "/MUSICIANS/C/Clever_Music/",				.filter = 0.0	} },
{ "David Dunn",			{	.folder = "/MUSICIANS/D/Dunn_David/",				.filter = -0.1	} },
{ "David Whittaker",	{	.folder = "/MUSICIANS/W/Whittaker_David/",			.filter = 0.0	} },
{ "DRAX",				{	.folder = "/MUSICIANS/D/DRAX/",						.filter = 0.55	} },
{ "Edwin van Santen",	{	.folder = "/MUSICIANS/0-9/20CC/van_Santen_Edwin/",	.filter = 0.35	} },
{ "Falco Paul",			{	.folder = "/MUSICIANS/0-9/20CC/Paul_Falco/",		.filter = -0.1	} },
{ "Figge Wasberger",	{	.folder = "/MUSICIANS/F/Fegolhuzz/",				.filter = -0.1	} },
{ "Fred Gray",			{	.folder = "/MUSICIANS/G/Gray_Fred/",				.filter = -0.25	} },
{ "Future Freak",		{	.folder = "/MUSICIANS/F/Future_Freak/",				.filter = 0.7	} },
{ "Geir Tjelta",		{	.folder = "/MUSICIANS/T/Tjelta_Geir/",				.filter = 0.4	} },
{ "Georg Feil",			{	.folder = "/MUSICIANS/F/Feil_Georg/",				.filter = -0.25	} },
{ "Glenn Gallefoos",	{	.folder = "/MUSICIANS/B/Blues_Muz/Gallefoss_Glenn/",.filter = -0.25	} },
{ "Jason Page",			{	.folder = "/MUSICIANS/P/Page_Jason/",				.filter = 0.0	} },
{ "Jeroen Tel",			{	.folder = "/MUSICIANS/T/Tel_Jeroen/",				.filter = 0.3	} },
{ "Johannes Bjerregaard",{	.folder = "/MUSICIANS/B/Bjerregaard_Johannes/",		.filter = 0.3	} },
{ "Johathan Dunn",		{	.folder = "/MUSICIANS/D/Dunn_Jonathan/",			.filter = 0.25	} },
{ "Jouni Ikonen",		{	.folder = "/MUSICIANS/M/Mixer/",					.filter = 0.4	} },
{ "Laxity",				{	.folder = "/MUSICIANS/L/Laxity/",					.filter = -0.55	} },
{ "Lft",				{	.folder = "/MUSICIANS/L/Lft/",						.filter = 0.3	} },
{ "Mark Cooksey",		{	.folder = "/MUSICIANS/C/Cooksey_Mark/",				.filter = 0.25	} },
{ "Markus Mueller",		{	.folder = "/MUSICIANS/M/Mueller_Markus/",			.filter = 0.2	} },
{ "Martin Galway",		{	.folder = "/MUSICIANS/G/Galway_Martin/",			.filter = 0.5	} },
{ "Martin Walker",		{	.folder = "/MUSICIANS/W/Walker_Martin/",			.filter = 0.0	} },
{ "Matt Gray",			{	.folder = "/MUSICIANS/G/Gray_Matt/",				.filter = -0.1	} },
{ "Michael Hendriks",	{	.folder = "/MUSICIANS/F/FAME/Hendriks_Michael/",	.filter = 0.1	} },
{ "Neil Brennan",		{	.folder = "/MUSICIANS/B/Brennan_Neil/",				.filter = 0.25	} },
{ "Peter Clarke",		{	.folder = "/MUSICIANS/C/Clarke_Peter/",				.filter = 0.4	} },
{ "Pex Tufvesson",		{	.folder = "/MUSICIANS/M/Mahoney/",					.filter = 0.6	} },
{ "Reyn Ouwehand",		{	.folder = "/MUSICIANS/O/Ouwehand_Reyn/",			.filter = 0.0	} },
{ "Richard Joseph",		{	.folder = "/MUSICIANS/J/Joseph_Richard/",			.filter = 0.3	} },
{ "Rob Hubbard",		{	.folder = "/MUSICIANS/H/Hubbard_Rob/",				.filter = 0.4,	.exceptions { {	"BMX_Kidz", "Yip"	} } } }, 			// Only Yip used the filter for subtune 4
{ "Russell Lieblich",	{	.folder = "/MUSICIANS/L/Lieblich_Russell/",			.filter = 0.6	} },
{ "Steve Turner",		{	.folder = "/MUSICIANS/T/Turner_Steve/",				.filter = 0.45,	.exceptions { { "Bushido", "Jason Page"	} } } },		// Only Jason Page used the filter for subtune 2
{ "Tim Follin",			{	.folder = "/MUSICIANS/F/Follin_Tim/",				.filter = 0.7	} },
{ "Yip",				{	.folder = "/MUSICIANS/Y/Yip/",						.filter = 0.5	} },
