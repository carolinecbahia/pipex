/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccavalca <ccavalca@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 16:04:39 by ccavalca          #+#    #+#             */
/*   Updated: 2025/12/14 16:09:37 by ccavalca         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <stdlib.h>
# include "libft.h"
# include "ft_printf/ft_printf.h"

typedef struct s_args
{
	char	*arg;
	t_args	*next;
	t_args	*prev;
}	t_args;

#endif