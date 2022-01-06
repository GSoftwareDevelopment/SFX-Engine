CONST
  CONST_SFX_ORG                        = 'SFX_ORG';
  CONST_AUDIO_BUFFER_ADDR              = 'AUDIO_BUFFER_ADDR';
  CONST_SFX_REGISTERS                  = 'SFX_REGISTERS';
  CONST_SFX_CHANNELS_ADDR              = 'SFX_CHANNELS_ADDR';
  CONST_NOTE_TABLE_PAGE                = 'NOTE_TABLE_PAGE';
  CONST_NOTE_TABLE_ADDR                = 'NOTE_TABLE_ADDR';
  CONST_SFX_DATA_ADDR                  = 'SFX_DATA_ADDR';
  CONST_SONG_ADDR                      = 'SONG_ADDR';
  CONST_SFX_MODE_SET_ADDR              = 'SFX_MODE_SET_ADDR';
  CONST_SFX_NOTE_SET_ADDR              = 'SFX_NOTE_SET_ADDR';
  CONST_SFX_TABLE_ADDR                 = 'SFX_TABLE_ADDR';
  CONST_TAB_TABLE_ADDR                 = 'TAB_TABLE_ADDR';

  COMMENT                              = '// ';
  INDENT                               = '	';

  COMMENT_SFX_SWITCH_ROM               = 'uncomment this line, if you want use RAM under ROM for data';
  COMMENT_SFX_SYNCAUDIOOUT             = 'this switch turn on buffer for POKEY data. Buffer is flush after processing all channels.';
  COMMENT_SFX_PLAYBACK                 = 'if you want use song player, leave this definition uncomment';

  CONF_CONST_SET_ABSOLUTE              = '%-17s = $%.*x;';
  CONF_CONST_SET_ORG_RELATIVE          = '%-17s = '+CONST_SFX_ORG+' + $%.4x;';
  CONF_CONST_SET_PAGE                  = '%-17s = hi(%s);';

	LABEL_NOTE_TABLES                    = 'note_tables';
  LABEL_NOTE_TABLE_N                   = 'note_table_%d';
	LABEL_DATA_SFX_N                     = 'data_sfx_%d';
	LABEL_DATA_TAB_N                     = 'data_tab_%d';
  LABEL_SFX_TABLE                      = 'sfxptr_table';
	LABEL_TAB_TABLE                      = 'tabptr_table';
	LABEL_SONG_DATA                      = 'song_table';
	LABEL_SFX_MODES_TABLE                = 'sfx_modes_table';
	LABEL_SFX_NOTES_TABLE                = 'sfx_notes_table';
	LABEL_DATA                           = 'data';

  ASM_COMMENT                          = '; ';
  ASM_ORG                              = 'org $%.4x';
  ASM_DTA                              = 'dta ';
  ASM_DTA_BYTE                         = '$%.2x';
  ASM_DTA_ADDRESS                      = 'a(%s)';
  ASM_DTA_SEPARATOR                    = ', ';
  COMMENT_DATA_OFFSET                  = 'offset %.4x';
  COMMENT_DATA_NOT_DEF                 = '%s #%d not defined';
  COMMENT_DATA_REINDEXED               = 'reindexed from %d to %d';
  COMMENT_BLOCK_INFO                   = 'block address: $%.4x length: $%.4x';
  RESOURCE_ENTRY                       = '%s rcasm ''%s'';';
  RESOURCE_PATH                        = 'RESOURCE.RESOURCE.RCASM0.RESOURCE._SFX_API.%s';

  DEFINE_ENTRY                         = '{$DEFINE %s}	%s';
	DEFINE_SFX_SWITCH_ROM                = 'SFX_SWITCH_ROM';
	DEFINE_SFX_SYNC_AUDIO_OUT            = 'SFX_SYNCAUDIOOUT';
	DEFINE_SFX_PLAYBACK                  = 'SFX_PLAYBACK';

	DEFINE_USE_MODULATORS                = 'USE_MODULATORS';
  DEFINE_MOD_DFD                       = 'DFD_MOD';
	DEFINE_MOD_LFD_NLM                   = 'LFD_NLM_MOD';
	DEFINE_MOD_MFD                       = 'MFD_MOD';
	DEFINE_MOD_HFD                       = 'HFD_MOD';

  CONST_ABSOLUTE                       = 0;
  CONST_ORG_RELATIVE                   = 1;
  CONST_PAGE                           = 2;
