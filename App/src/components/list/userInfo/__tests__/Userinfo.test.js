import React from 'react';
import { render } from '@testing-library/react';
import '@testing-library/jest-dom';
import Userinfo from '../Userinfo';
import { useUserStore } from '../../../../lib/userStore';

jest.mock('../../../../lib/userStore');

describe('Userinfo Component', () => {
    it('renders user information correctly', () => {
        const mockUser = {
            avatar: 'https://example.com/avatar.png',
            username: 'testuser',
            bio: 'This is a test bio'
        };

        useUserStore.mockReturnValue({ currentUser: mockUser });

        const { getByText, getByAltText } = render(<Userinfo />);

        expect(getByAltText('')).toHaveAttribute('src', mockUser.avatar);
        expect(getByText(mockUser.username)).toBeInTheDocument();
        expect(getByText(mockUser.bio)).toBeInTheDocument();
    });

    it('renders default avatar and bio when not provided', () => {
        const mockUser = {
            avatar: '',
            username: 'testuser',
            bio: ''
        };

        useUserStore.mockReturnValue({ currentUser: mockUser });

        const { getByText, getByAltText } = render(<Userinfo />);

        expect(getByAltText('')).toHaveAttribute('src', './avatar.png');
        expect(getByText(mockUser.username)).toBeInTheDocument();
        expect(getByText('No bio yet')).toBeInTheDocument();
    });
});