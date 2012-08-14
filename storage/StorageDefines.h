/*
 * Copyright (c) [2004-2009] Novell, Inc.
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef STORAGE_DEFINES_H
#define STORAGE_DEFINES_H


#define SYSFSDIR "/sys/block"

#define SYSCONFIGFILE "/etc/sysconfig/storage"

#define PARTEDBIN "/usr/sbin/parted"
#define PARTEDCMD PARTEDBIN " -s "	// blank at end !!
#define LABEL_GPT_SYNC_MBR "gpt_sync_mbr"
#define ADDPARTBIN "/usr/sbin/addpart"
#define DELPARTBIN "/usr/sbin/delpart"

#define MDADMBIN "/sbin/mdadm"

#define PVCREATEBIN "/sbin/pvcreate"

#define LVCREATEBIN "/sbin/lvcreate"
#define LVREMOVEBIN "/sbin/lvremove"
#define LVEXTENDBIN "/sbin/lvextend"
#define LVREDUCEBIN "/sbin/lvreduce"
#define LVSBIN "/sbin/lvs"

#define VGCREATEBIN "/sbin/vgcreate"
#define VGREMOVEBIN "/sbin/vgremove"
#define VGEXTENDBIN "/sbin/vgextend"
#define VGREDUCEBIN "/sbin/vgreduce"
#define VGDISPLAYBIN "/sbin/vgdisplay"
#define VGSBIN "/sbin/vgs"
#define VGSCANBIN "/sbin/vgscan"
#define VGCHANGEBIN "/sbin/vgchange"

#define CRYPTSETUPBIN "/sbin/cryptsetup"
#define LOSETUPBIN "/sbin/losetup"
#define MULTIPATHBIN "/sbin/multipath"
#define MULTIPATHDBIN "/sbin/multipathd"
#define DMSETUPBIN "/sbin/dmsetup"
#define DMRAIDBIN "/sbin/dmraid"
#define BTRFSBIN "/sbin/btrfs"
#define WIPEFSBIN "/sbin/wipefs"

#define MOUNTBIN "/bin/mount"
#define UMOUNTBIN "/bin/umount"
#define SWAPONBIN "/sbin/swapon"
#define SWAPOFFBIN "/sbin/swapoff"

#define DDBIN "/bin/dd"

#define BLKIDBIN "/sbin/blkid"
#define BLOCKDEVBIN "/sbin/blockdev"
#define LSSCSIBIN "/usr/bin/lsscsi"

#define LSBIN "/bin/ls"

#define DASDFMTBIN "/sbin/dasdfmt"
#define DASDVIEWBIN "/sbin/dasdview"
#define FDASDBIN "/sbin/fdasd"

#define UDEVADMBIN "/sbin/udevadm"

#define MODPROBEBIN "/sbin/modprobe"
#define GREPBIN        "/usr/bin/grep"
#define PORTMAPBIN     "/sbin/portmap"
#define RPCBINDBIN     "/sbin/rpcbind"

#define NTFSRESIZEBIN "/usr/sbin/ntfsresize"
#define XFSGROWFSBIN  "/usr/sbin/xfs_growfs"
#define REISERRESBIN  "/sbin/resize_reiserfs"
#define EXT2RESIZEBIN "/sbin/resize2fs"

#define TUNE2FSBIN     "/sbin/tune2fs"
#define TUNEREISERBIN  "/sbin/reiserfstune"
#define TUNEJFSBIN     "/sbin/jfs_tune"
#define XFSADMINBIN    "/usr/sbin/xfs_admin"
#define NTFSLABEL      "/usr/sbin/ntfslabel"

#define FSCKBIN        "/sbin/fsck"
#define FSCKEXT2BIN    "/sbin/fsck.ext2"
#define FSCKEXT3BIN    "/sbin/fsck.ext3"
#define FSCKEXT4BIN    "/sbin/fsck.ext4"
#define FSCKJFSBIN     "/sbin/fsck.jfs"
#define FSCKXFSBIN     "/usr/sbin/xfs_check"
#define FSCKREISERBIN  "/sbin/reiserfsck"

#define MKSWAPBIN      "/sbin/mkswap"
#define HFORMATBIN     "/usr/bin/hformat"
#define MKFSXFSBIN     "/sbin/mkfs.xfs"
#define MKFSFATBIN     "/sbin/mkdosfs"
#define MKFSJFSBIN     "/sbin/mkfs.jfs"
#define MKFSREISERBIN  "/sbin/mkreiserfs"
#define MKFSEXT2BIN    "/sbin/mke2fs"
#define MKFSBTRFSBIN   "/sbin/mkfs.btrfs"

#endif
