/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssfar <samisfar.dev@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/17 13:27:02 by ssfar             #+#    #+#             */
/*   Updated: 2020/07/24 13:09:03 by ssfar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "corewar_vm.h"

int		main(int ac, char **av)
{
	corewar_vm(ac, av);
}

void	corewar_vm(int ac, char **av)
{
	t_vm	vm;

	vm.process = NULL;
	vm.player = NULL;
	vm.window = NULL;
	vm.nb_player = 0;
	if (ac < 2)
		exit_failure(&vm, "Usage : ./corewar -v "
		"[-dump nbr_cycles] [[-n number] champion1.cor] ...", NULL, false);
	init(ac, av, &vm);
	parsing(ac, av, &vm);
	complete_player_id(ac, av, &vm);
	ft_bzero(vm.arena, MEM_SIZE);
	read_player_file(&vm);
	print_player(&vm);
	if (vm.visualiser == true)
		init_visu(&vm);
	game_loop(&vm);
	print_winner(&vm);
	free_all(&vm);
}

/*
** Initialise most of the program structures, count the number of players and
** malloc the player structure.
*/

void	init(int ac, char **av, t_vm *vm)
{
	int	i;

	vm->visualiser = false;
	vm->dump = false;
	vm->nbr_live_reached = false;
	vm->cycle_to_die = CYCLE_TO_DIE;
	vm->cycle_counter = 0;
	vm->check_counter = 0;
	vm->live_counter = 0;
	vm->option_id = 0;
	count_player(ac, av, vm);
	if (!(vm->player = malloc(sizeof((*vm->player)) * vm->nb_player)))
		exit_failure(vm, "Malloc error", NULL, false);
	i = 0;
	while (i < vm->nb_player)
	{
		vm->player[i].file = NULL;
		i++;
	}
	vm->last_reported_alive = -i;
	load_function(vm);
	load_cost(vm);
}

/*
** Assign a player identification number to thoses without one, in the order of
** availability.
*/

void	complete_player_id(int ac, char **av, t_vm *vm)
{
	int				i;
	int				j;

	if (vm->option_id != 0)
		exit_failure(vm, "Usage : ./corewar -v "
		"[-dump nbr_cycles] [[-n number] champion1.cor] ...", NULL, false);
	i = 0;
	while (i < ac)
	{
		if (ft_strlen(av[i]) > 4
			&& ft_strcmp(&(av[i][ft_strlen(av[i]) - 4]), ".cor") == 0)
		{
			j = 0;
			while (j < vm->nb_player && vm->player[j].file != av[i])
				j++;
			if (j == vm->nb_player)
			{
				j = 0;
				while (vm->player[j].file != NULL)
					j++;
				vm->player[j].file = av[i];
			}
		}
		i++;
	}
}

/*
** Free malloced memory, print a specific error message and close the program.
*/

void	exit_failure(t_vm *vm, char *error_message, char *file,
			t_bool call_perror)
{
	free_all(vm);
	if (call_perror == true)
		perror(error_message);
	else if (file != NULL)
	{
		ft_dprintf(2, error_message, file);
		write(2, "\n", 1);
	}
	else if (error_message)
	{
		write(2, error_message, ft_strlen(error_message));
		write(2, "\n", 1);
	}
	exit(-1);
}
