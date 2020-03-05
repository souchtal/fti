.. Fault Tolerance Library documentation Configuration file



[Basic]
===================================================
The checkpointing safety levels L2, L3 and L4 produce additional overhead due to the necessary postprocessing work on the checkpoints. FTI offers the possibility to create an MPI process, called HEAD, in which this postprocessing will be accomplished. This allows it for the application processes to continue the execution immediately after the checkpointing.

head
===================================================


node_size
ckpt_dir
glbl_dir
meta_dir
ckpt_L1
ckpt_L2
ckpt_L3
ckpt_L4
dcp_L4
inline_L2
inline_L3
inline_L4
keep_last_ckpt
keep_l4_ckpt
group_size
max_synch_intv
ckpt_io
enable_staging
enable_dcp
dcp_mode
dcp_block_size
verbosity
[Restart]
failure
exec_id
[Advanced]
block_size
transfer_size
general_tag
ckpt_tag
stage_tag
final_tag
local_test
lustre_striping_unit
lustre_striping_factor
lustre_striping_offset