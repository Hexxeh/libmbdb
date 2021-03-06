/**
  * libmbdb-1.0 - mbdbtool.c
  * Copyright (C) 2013 Crippy-Dev Team
  * Copyright (C) 2011-2013 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libmbdb-1.0/backup.h>
#include <libcrippy-1.0/libcrippy.h>

int main(int argc, char* argv[]) {
	if (argc < 5) {
		printf("usage: mbdbtool <dir> <uuid> <domain> <cmd> [args]\n");
		return 0;
	}

	char* dir = strdup(argv[1]);
	char* udid = strdup(argv[2]);
	char* dom = strdup(argv[3]);
	char* cmd = strdup(argv[4]);

	printf("Found device with UUID %s\n", udid);

	if (strcmp(cmd, "ls") == 0) {
		if (argc != 6) {
			printf("usage: mbdbtool <dir> <domain> ls <path>\n");
			free(udid);
			free(cmd);
			free(dir);
			free(dom);
			return 0;
		}
		backup_t* backup = backup_open(dir, udid);

		// List the directory
		backup_free(backup);
	} else if (strcmp(cmd, "get") == 0) {
		if (argc != 7) {
			printf("usage: mbdbtool <dir> <domain> get <remote> <local>\n");
			free(udid);
			free(cmd);
			free(dir);
			free(dom);
			return 0;
		}
		backup_t* backup = backup_open(dir, udid);
		if (backup) {
			printf("Backup opened\n");
			backup_file_t* file = backup_get_file(backup, dom, argv[5]);
			if (file) {
				char* path = backup_get_file_path(backup, file);
				printf("Got file %s\n", path);
				unsigned int size = 0;
				unsigned char* data = NULL;
				file_read(path, &data, &size);
				file_write(argv[6], data, size);
				free(data);
				backup_file_free(file);
			}
			backup_free(backup);
		}
	} else if (strcmp(cmd, "put") == 0) {
		if (argc != 7) {
			printf("usage: mbdbtool <dir> <uuid> <domain> put <local> <remote>\n");
			free(udid);
			free(cmd);
			free(dir);
			free(dom);
			return 0;
		}
		backup_t* backup = backup_open(dir, udid);
		if (backup) {
			printf("Backup opened\n");
			unsigned int size = 0;
			unsigned char* data = NULL;
			file_read(argv[5], &data, &size);
			backup_file_t* file = backup_get_file(backup, dom, argv[6]);
			if (file) {
				printf("Found file, replacing it\n");
				char* fn = backup_get_file_path(backup, file);
				if (fn != NULL ) {
					file_write(fn, &data, &size);
					backup_file_assign_file_data(file, data, size, 1);
					backup_file_set_length(file, size);
					backup_file_update_hash(file);
					backup_update_file(backup, file);
					backup_write_mbdb(backup);
					backup_file_free(file);
				} else {
					printf("Error, unable to find file in backup\n");
				}
			} else {
				printf("File not found, creating new file\n");

				unsigned int tm = (unsigned int) (time(NULL ));
				file = backup_file_create_with_data(data, size, 0);
				backup_file_set_domain(file, dom);
				backup_file_set_path(file, argv[6]);
				backup_file_set_mode(file, 0100644);
				backup_file_set_inode(file, 123456);
				backup_file_set_uid(file, 0);
				backup_file_set_gid(file, 0);
				backup_file_set_time1(file, tm);
				backup_file_set_time2(file, tm);
				backup_file_set_time3(file, tm);
				backup_file_set_length(file, size);
				backup_file_set_flag(file, 4);
				backup_file_update_hash(file);
				backup_update_file(backup, file);
				backup_write_mbdb(backup);
				backup_file_free(file);
			}
			backup_free(backup);
		}
	} else if (strcmp(cmd, "symlink") == 0) {
		if (argc != 7) {
			printf("usage: mbdbtool <dir> <uuid> <domain> symlink <from> <to>\n");
			free(udid);
			free(cmd);
			free(dir);
			free(dom);
			return 0;
		}
		backup_t* backup = backup_open(dir, udid);
		if (backup) {
			printf("Backup opened\n");
			unsigned int size = 0;
			unsigned char* data = NULL;
			backup_file_t* file = backup_get_file(backup, dom, argv[5]);
			symlink(argv[6], "./test");
			file_read("./test", &data, &size);
			if (file) {
				printf("Found file, replacing it\n");
				char* fn = backup_get_file_path(backup, file);
				if (fn != NULL ) {
					backup_file_set_mode(file, 0120000);
					backup_file_set_target(file, argv[6]);
					backup_file_assign_file_data(file, data, size, 1);
					backup_file_set_length(file, size);
					backup_file_update_hash(file);
					backup_update_file(backup, file);
					backup_write_mbdb(backup);
					backup_file_free(file);
				} else {
					printf("Error, unable to find file in backup\n");
				}
			} else {
				printf("File not found, creating new file\n");
				unsigned int tm = (unsigned int) (time(NULL ));
				file = backup_file_create_with_data(data, size, 0);
				backup_file_set_target(file, argv[6]);
				backup_file_set_domain(file, dom);
				backup_file_set_path(file, argv[5]);
				backup_file_set_mode(file, 0120777);
				backup_file_set_inode(file, 123456);
				backup_file_set_uid(file, 0);
				backup_file_set_gid(file, 0);
				backup_file_set_time1(file, tm);
				backup_file_set_time2(file, tm);
				backup_file_set_time3(file, tm);
				backup_file_set_length(file, size);
				backup_file_set_flag(file, 4);
				backup_file_update_hash(file);
				backup_update_file(backup, file);
				backup_write_mbdb(backup);
				backup_file_free(file);
			}
			unlink("./test");
			backup_free(backup);
		}
	} else if (strcmp(cmd, "mkdir") == 0) {
		if(argc != 6) {
			printf("usage: mbdbtool <dir> <uuid> <domain> mkdir <path>\n");
			free(udid);
			free(cmd);
			free(dir);
			free(dom);
			return 0;
		}
		backup_t* backup = backup_open(dir, udid);
		if(backup) {
			printf("Backup opened\n");
			backup_mkdir(backup, dom, argv[5], 0777, 0, 0, 4);
			backup_free(backup);
		} 
	} else if (strcmp(cmd, "rm") == 0) {
		if (argc != 6) {
			printf("usage: mbdbtool <dir> <domain> rm <remote>\n");
			free(udid);
			free(cmd);
			free(dir);
			free(dom);
			return 0;
		}
		backup_t* backup = backup_open(dir, udid);
		if (backup) {
			printf("Backup opened\n");
			backup_file_t* file = backup_get_file(backup, dom, argv[5]);
			if (file) {
				char* path = backup_get_file_path(backup, file);
				backup_remove_file(backup, file);
				unlink(path);
				printf("Removed file %s\n", path);
				backup_write_mbdb(backup);
				backup_file_free(file);
			}
			backup_free(backup);
		}
	} else {
		printf("Unknown command %s\n", cmd);
	}

	if(udid) free(udid);
	if(cmd) free(cmd);
	if(dir) free(dir);
	if(dom) free(dom);
	return 0;
}
