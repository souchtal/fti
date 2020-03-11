.. Fault Tolerance Library documentation Configuration examples file

Configuration Examples
=================================

Default Configuration
----------------------------------
.. code-block:: bash

	[basic]
	head                           = 0
	node_size                      = 2
	ckpt_dir                       = ./Local
	glbl_dir                       = ./Global
	meta_dir                       = ./Meta
	ckpt_l1                        = 3
	ckpt_l2                        = 5
	ckpt_l3                        = 7
	ckpt_l4                        = 11
	dcp_l4                         = 0
	inline_l2                      = 1
	inline_l3                      = 1
	inline_l4                      = 1
	keep_last_ckpt                 = 0
	keep_l4_ckpt                   = 0
	group_size                     = 4
	max_sync_intv                  = 0
	ckpt_io                        = 1
	enable_staging                 = 0
	enable_dcp                     = 0
	dcp_mode                       = 0
	dcp_block_size                 = 16384
	verbosity                      = 2

	[restart]
	failure                        = 0
	exec_id                        = 2018-09-17_09-50-30

	[injection]
	rank                           = 0
	number                         = 0
	position                       = 0
	frequency                      = 0

	[advanced]
	block_size                     = 1024
	transfer_size                  = 16
	general_tag                    = 2612
	ckpt_tag                       = 711
	stage_tag                      = 406
	final_tag                      = 3107
	local_test                     = 1
	lustre_striping_unit           = 4194304
	lustre_striping_factor         = -1
	lustre_striping_offset         = -1

**DESCRIPTION**  

..

   This configuration is made of default values (see: 5). FTI processes are not created (\ ``head = 0``\ , notice: if there is no FTI processes, all post-checkpoints must be done by application processes, thus ``inline_L2``\ , ``inline_L3`` and ``inline_L4`` are set to 1), last checkpoint won’t be kept (\ ``keep_last_ckpt = 0``\ ), ``FTI_Snapshot()`` will take L1 checkpoint every 3 min,L2 - every 5 min, L3 - every 7 min and L4 - every 11 min, FTI will print errors and some few important information (\ ``verbosity = 2``\ ) and IO mode is set to POSIX (\ ``ckpt_io = 1``\ ). This is a normal launch of a job, because failure is set to 0 and ``exec_id`` is ``NULL``. ``local_test = 1`` makes this a local test.  