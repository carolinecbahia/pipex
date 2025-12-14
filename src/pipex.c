/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccavalca <ccavalca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 16:07:28 by ccavalca          #+#    #+#             */
/*   Updated: 2025/12/14 16:15:43 by ccavalca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	args_parse(t_args *args, char **av)
{
	int		i;
	char	**temp;

	i = 0;
	while (av[i])
	{
		temp = ft_split(av[i], ' ');
	}
}

int	main(int argc, char **argv)
{
	int		i;
	t_args	*args;

	if (argc != 4)
		ft_printf("Error.\nSem argumentos suficientes.\n");
	args_parse(args, argv);
	while (i < lst_size(args))
	{
		ft_printf("%s", args);
		args = args->next;
	}
}
