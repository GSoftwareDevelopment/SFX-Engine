const
	VERSION = '1.0.5';

	EOL = #10#13;

	SFXMM_VER1_1 = $11;
	SFXMM_VER1_2 = $12;

	maxSFXs = 64;
	maxTABs = 64;
	maxNoteTables = 4;

	SFXNameLength = 14;
	TABNameLength = 8;
	SONGNameLength = 32;
	NOTETABnameLength = 11;

	section_main   :TTag = (83, 70, 88, 77, 77); // SFXMM
	section_SFX    :TTag = ( 0,  0, 83, 70, 88); // __SFX
	section_NOTE   :TTag = ( 0, 78, 79, 84, 69); // _NOTE
	section_TAB    :TTag = ( 0,  0, 84, 65, 66); // __TAB
	section_SONG   :TTag = ( 0, 83, 79, 78, 71); // _SONG

	DEFAULT_CONFIG_FILENAME = 'sfx_engine.conf.inc';
	DEFAULT_RESOURCE_FILENAME = 'resource.rc';
	DEFAULT_DATA_FILENAME = 'data.asm';
	DEFAULT_NOTE_TABLES_FILENAME = 'notetables.asm';
	DEFAULT_SONG_TABLE_FILENAME = 'song.asm';
	DEFAULT_SFX_MODES_TABLE_FILENAME = 'modesset.asm';
	DEFAULT_SFX_NOTES_TABLE_FILENAME = 'notesset.asm';
	DEFAULT_SFX_TABLE_FILENAME = 'sfxtable.asm';
	DEFAULT_TAB_TABLE_FILENAME = 'tabtable.asm';

	DEFAULT_ORIGIN = $A000;
